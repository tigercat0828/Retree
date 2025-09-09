#include "pch.h"
#include "Retree/TreeMeshBuilder.h"
#include "Retree/PosGraph.h"
#include "Retree/Tree.h"
using glm::vec3;
using glm::vec2;
using glm::mat3;

BranchMesh TreeMeshBuilder::MakeBranch(const vec3& p1, const vec3& p2, float r1, float r2, int segments) {
	/*
	i   n
	0---1
	| \ |
	3---2
	*/
	segments = std::max(3, segments);
	constexpr float PI = glm::pi<float>();
	constexpr float _2PI = 2.0f * glm::pi<float>();
	const float radius = 0.1f;

	vec3 UP(0, 1, 0);
	vec3 axis = glm::normalize(p2 - p1);
	if (glm::abs(glm::dot(axis, UP)) > 0.99) UP = vec3(1, 0, 0);

	vec3 tangent = glm::normalize(glm::cross(UP, axis));
	vec3 bitangent = glm::normalize(glm::cross(axis, tangent));

	vector<vec3> topCircle;	// top circle
	vector<vec3> botCircle;	// bottom circle

	for (int i = 0; i < segments; i++) {
		float theta = i * 2.0f * PI / segments;
		glm::vec3 baseOffset = r1 * (cos(theta) * tangent + sin(theta) * bitangent);
		glm::vec3 topOffset = r2 * (cos(theta) * tangent + sin(theta) * bitangent);
		botCircle.push_back(p1 + baseOffset);
		topCircle.push_back(p2 + topOffset);
	}
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> texCoords;
	// side faces
	float angle = _2PI / segments;
	for (int i = 0; i < segments; i++) {
		int n = (i + 1) % segments;	// next

		positions.emplace_back(topCircle[i]);  // 0
		positions.emplace_back(topCircle[n]);   // 1
		positions.emplace_back(botCircle[n]);   // 2
		positions.emplace_back(topCircle[i]);   // 0
		positions.emplace_back(botCircle[n]);   // 2
		positions.emplace_back(botCircle[i]);   // 3

		// normals should be 角平分
		normals.emplace_back(glm::normalize(topCircle[i] - p2));   // 0
		normals.emplace_back(glm::normalize(topCircle[n] - p2));   // 1
		normals.emplace_back(glm::normalize(botCircle[n] - p1));   // 2
		normals.emplace_back(glm::normalize(topCircle[i] - p2));   // 0
		normals.emplace_back(glm::normalize(botCircle[n] - p1));   // 2
		normals.emplace_back(glm::normalize(botCircle[i] - p1));   // 3

		float theta0 = i * _2PI / segments;
		float theta1 = (i + 1) * _2PI / segments;
		float u0 = TexRepeat * theta0 / _2PI;
		float u1 = TexRepeat * theta1 / _2PI;

		texCoords.emplace_back(u0, 1.0f);
		texCoords.emplace_back(u1, 1.0f);
		texCoords.emplace_back(u1, 0.0f);
		texCoords.emplace_back(u0, 1.0f);
		texCoords.emplace_back(u1, 0.0f);
		texCoords.emplace_back(u0, 0.0f);
	}
	return { positions, normals, texCoords };
}

BranchMesh TreeMeshBuilder::MakePipe(const vector<pair<vec3, float>>& path, int segments) {
    BranchMesh mesh;
    int psize = path.size();
    if (psize < 2) return mesh;  // 若不足兩點，無法建構管子

    constexpr float PI = glm::pi<float>();
    constexpr float _2PI = 2.0f * PI;
    segments = std::max(3, segments);  // 至少三邊，才能近似圓形截面

    // Perform (Parallel Transport Frames) 
    
    struct Frame {          // 每個截面需要一個局部座標
        vec3 tangent;       // 方向
        vec3 normal;        // 初始旋轉方向
        vec3 bitangent;     // 法線
    };
    
    // 使用羅德里格旋轉公式旋轉向量 ---
    auto RotateVecRodrigues = [](const vec3& v, const vec3& axis, float angle) -> vec3 {
        // Rodrigues' rotation formula: v_rot = v*cosθ + (axis×v)*sinθ + axis*(axis·v)*(1-cosθ)
        return v * cos(angle) + glm::cross(axis, v) * sin(angle) + axis * glm::dot(axis, v) * (1.0f - cos(angle));
    };

    // Step 1：為每個點建立平行移動框架 (Parallel Transport Frames)
    vector<Frame> frames(psize);

    for (int i = 0; i + 1 < psize; ++i) {
        vec3 p0 = path[i].first;
        vec3 p1 = path[i + 1].first;
        vec3 T1 = glm::normalize(p1 - p0);  // 計算本段切線方向

        if (i == 0) {
            // --- 初始切線：第一次要找一個穩定參考軸，避免平行退化 ---
            vec3 ref = (glm::abs(glm::dot(T1, vec3(0, 1, 0))) < 0.95f) ? vec3(0, 1, 0) : vec3(1, 0, 0);
            vec3 N = glm::normalize(glm::cross(T1, ref));   // normal 是與切線垂直的方向
            vec3 B = glm::normalize(glm::cross(T1, N));     // bitangent = T x N，保證正交
            frames[i] = { T1, N, B };
        }

        // --- 平行移動核心：根據新切線 T1，調整原本的 normal 方向 ---
        vec3 T0 = frames[i].tangent;  // 前一段切線
        vec3 N0 = frames[i].normal;   // 前一段 normal

        // 計算 T0 與 T1 之間的旋轉軸與角度
        vec3 v = glm::cross(T0, T1);   // 旋轉軸
        float c = glm::dot(T0, T1);    // cosθ
        float s = glm::length(v);      // sinθ

        vec3 N1;
        if (s < 1e-5f)  N1 = N0;   // 若兩段方向幾乎相同，則不旋轉
            
        else {      // 執行平行移動旋轉：把 N0 沿旋轉軸轉到與 T1 共面但垂直
            vec3 axis = glm::normalize(v);         // 單位旋轉軸
            float angle = atan2(s, c);             // 計算旋轉角度 θ
            N1 = glm::normalize(RotateVecRodrigues(N0, axis, angle));
        }

        // bitangent 永遠是 N1 與 T1 的叉積，保證正交
        vec3 B1 = glm::normalize(glm::cross(T1, N1));
        frames[i + 1] = { T1, N1, B1 };
    }

    // Step2：每個點生成圓形截面
    vector<vector<vec3>> circles;
    for (int i = 0; i < psize; ++i) {
        const auto& center = path[i].first;
        float radius = path[i].second;
        const Frame& f = frames[i];

        vector<vec3> circle;
        for (int j = 0; j < segments; ++j) {
            
            float theta = j * _2PI / segments;      // 每一段角度 (0 ~ 2PI)
            vec3 offset = radius * (cos(theta) * f.normal + sin(theta) * f.bitangent);  // 在 normal / bitangent 平面上做圓周運動
            circle.push_back(center + offset);
        }
        circles.push_back(std::move(circle));  
    }

    // --- 第三步：生成側面三角形 ---
    for (int i = 0; i + 1 < psize; ++i) {
        const auto& bot = circles[i];
        const auto& top = circles[i + 1];
        vec3 p0 = path[i].first;
        vec3 p1 = path[i + 1].first;

        for (int j = 0; j < segments; ++j) {
            int n = (j + 1) % segments;  // 下一個圓周點 (閉合)
            
            vec3 v0 = top[j];
            vec3 v1 = top[n];
            vec3 v2 = bot[n];
            vec3 v3 = bot[j];

            mesh.positions.insert(mesh.positions.end(), { v0, v1, v2, v0, v2, v3 });

            vec3 n0 = glm::normalize(top[j] - p1);
            vec3 n1 = glm::normalize(top[n] - p1);
            vec3 n2 = glm::normalize(bot[n] - p0);
            vec3 n3 = glm::normalize(bot[j] - p0);
            mesh.normals.insert(mesh.normals.end(), { n0, n1, n2, n0, n2, n3 });

            // UV 映射：u 為角度，v 為長度方向
            float u0 = TexRepeat * float(j) / segments;
            float u1 = (n == 0) ? TexRepeat : TexRepeat * float(n) / segments;  // why ? 
            mesh.texCoords.emplace_back(u0, 1.0f);
            mesh.texCoords.emplace_back(u1, 1.0f);
            mesh.texCoords.emplace_back(u1, 0.0f);
            mesh.texCoords.emplace_back(u0, 1.0f);
            mesh.texCoords.emplace_back(u1, 0.0f);
            mesh.texCoords.emplace_back(u0, 0.0f);
        }
    }

    return mesh;
}



BranchMesh TreeMeshBuilder::GenerateBranches(const Tree& tree, int segments) {
	auto pipeList = tree.GetPipeData();
	BranchMesh merged;

	for (const auto& path : pipeList) {
		
		 BranchMesh part = MakePipe(path , segments);

		merged.positions.insert(merged.positions.end(), part.positions.begin(), part.positions.end());
		merged.normals.insert(merged.normals.end(), part.normals.begin(), part.normals.end());
		merged.texCoords.insert(merged.texCoords.end(), part.texCoords.begin(), part.texCoords.end());
	}
	return merged;
}

BranchMesh TreeMeshBuilder::ConcatenateMeshes(const std::vector<BranchMesh>& meshes) {
	BranchMesh result;
	for (const auto& mesh : meshes) {
		result.positions.insert(result.positions.end(), mesh.positions.begin(), mesh.positions.end());
		result.normals.insert(result.normals.end(), mesh.normals.begin(), mesh.normals.end());
		result.texCoords.insert(result.texCoords.end(), mesh.texCoords.begin(), mesh.texCoords.end());
	}

	return result;
}

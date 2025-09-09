#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <utility>
using std::pair;
using glm::vec3;
using glm::vec2;
using std::vector;

class PosGraph;
class Tree;

struct BranchMesh {	// TODO : rename "BasicMesh" later
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> texCoords;
};

class TreeMeshBuilder {
public:
	inline static int TexRepeat = 4;

	static BranchMesh MakeBranch(const vec3& p1, const vec3& p2, float r1, float r2, int segments = 8);	// 可能還會用到
	static BranchMesh MakePipe(const vector<pair<vec3, float>>& path, int segments = 3);
	
	static BranchMesh GenerateBranches(const Tree& graph, int segments =8);
	static BranchMesh ConcatenateMeshes(const std::vector<BranchMesh>& meshes);
	
};

// wind shader 
/*
// 顶点着色器示例
uniform float time;
uniform float windStrength;

void main() {
	vec3 windEffect = vec3(0.0, sin(position.x * 2.0 + time) * windStrength, 0.0);
	gl_Position = projectionMatrix * modelViewMatrix * vec4(position + windEffect, 1.0);
}


// 片段着色器示例
void main() {
	vec4 color = texture2D(texture, uv);
	gl_FragColor = color * vec4(1.0, 1.0 - abs(sin(time)), 1.0, 1.0); // 模擬顏色變化
}
*/
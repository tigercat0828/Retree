#include "pch.h"
#include "Core/Primitives.h"
#include "Core/ShaderManager.h"
#include "Core/ResourceManager.h"
#include "Core/Camera.h"
using std::vector;
using glm::vec3;
using glm::vec2;
using glm::mat4;

/// <summary>
///  float array(positions:3 float, normals:3 float, texCoords:2 float)
/// </summary>
void Primitive_SetupGL_Helper_Array(
	GLuint& VAO, GLuint& PosVBO, GLuint& NorVBO, GLuint& TexVBO,
	float* positions, float* normals, float* texCoords,
	size_t positionsSize, size_t normalsSize, size_t texCoordsSize
) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &PosVBO);
	glGenBuffers(1, &NorVBO);
	glGenBuffers(1, &TexVBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, PosVBO);		// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, NorVBO);		// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, TexVBO);		// texture coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	// upload buffer
	glBindBuffer(GL_ARRAY_BUFFER, PosVBO);
	glBufferData(GL_ARRAY_BUFFER, positionsSize, positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NorVBO);
	glBufferData(GL_ARRAY_BUFFER, normalsSize, normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, TexVBO);
	glBufferData(GL_ARRAY_BUFFER, texCoordsSize, texCoords, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/// <summary>
///  vec3 vector(positions:vec3, normals:vec3, texCoords:vec2)
/// </summary>
void Primitive_SetupGL_Helper_Vector(
	GLuint& VAO, GLuint& PosVBO, GLuint& NorVBO, GLuint& TexVBO,
	const vector<vec3>& positions, const vector<vec3>& normals, const vector<vec2>& texCoords
) {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &PosVBO);
	glGenBuffers(1, &NorVBO);
	glGenBuffers(1, &TexVBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, PosVBO);		// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, NorVBO);		// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, TexVBO);		// texture coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
	glEnableVertexAttribArray(2);

	// upload buffer
	glBindBuffer(GL_ARRAY_BUFFER, PosVBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NorVBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, TexVBO);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data(), GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Cube::OnDraw(Camera& cam) {
	if (!AllowDraw) return;
	pTexture->Bind();
	pShader->Use();
	pShader->SetMat4("uModelMat", transform.GetModelMatrix());
	pShader->SetMat4("uViewMat", cam.GetViewMat());
	pShader->SetMat4("uProjMat", cam.GetProjMat());
	glBindVertexArray(sVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	pShader->Unuse();
}
void Cube::SetTexture(const std::string& name) {
	ResourceManager::LoadTexture(name);
	pTexture = ResourceManager::GetTexture(name);
}
void Plane::OnDraw(Camera& cam) {
	if(!AllowDraw) return;
	pTexture->Bind();
	pShader->Use();
	pShader->SetMat4("uModelMat", transform.GetModelMatrix());
	pShader->SetMat4("uViewMat", cam.GetViewMat());
	pShader->SetMat4("uProjMat", cam.GetProjMat());
	glBindVertexArray(sVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	pShader->Unuse();
}

void Cube::SetupGL() {
	// 立方体顶点数据 (每个面由2个三角形组成，共36个顶点)
	float positions[] = {
		-0.5f, -0.5f,  0.5f,   0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,  
		-0.5f, -0.5f,  0.5f,   0.5f,  0.5f,  0.5f,  -0.5f,  0.5f,  0.5f,  

		 0.5f, -0.5f, -0.5f,  -0.5f, -0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,  
		 0.5f, -0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,   0.5f,  0.5f, -0.5f,  

		 -0.5f, -0.5f, -0.5f,  -0.5f, -0.5f,  0.5f,  -0.5f,  0.5f,  0.5f, 
		 -0.5f, -0.5f, -0.5f,  -0.5f,  0.5f,  0.5f,  -0.5f,  0.5f, -0.5f, 

		  0.5f, -0.5f,  0.5f,   0.5f, -0.5f, -0.5f,   0.5f,  0.5f, -0.5f, 
		  0.5f, -0.5f,  0.5f,   0.5f,  0.5f, -0.5f,   0.5f,  0.5f,  0.5f, 

		  -0.5f,  0.5f,  0.5f,   0.5f,  0.5f,  0.5f,   0.5f,  0.5f, -0.5f,
		  -0.5f,  0.5f,  0.5f,   0.5f,  0.5f, -0.5f,  -0.5f,  0.5f, -0.5f,
		  
		  -0.5f, -0.5f, -0.5f,   0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,
		  -0.5f, -0.5f, -0.5f,   0.5f, -0.5f,  0.5f,  -0.5f, -0.5f,  0.5f 
	};

	float normals[] = {
		0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,

		-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f
	};

	float texCoords[] = {
		0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 
		0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 

		0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 
		0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 
		
		0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 
		0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 

		0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 
		0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 

		0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 
		0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 

		0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 
		0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f  
	};

	Primitive_SetupGL_Helper_Array(
		sVAO, sPosVBO, sNorVBO, sTexVBO,
		positions, normals, texCoords,
		sizeof(positions), sizeof(normals), sizeof(texCoords)
	);
}


void Plane::SetupGL() {
	float positions[] = {
		0.5f, 0.5f, 0.0f,		-0.5f, 0.5f, 0.0f,		-0.5f, -0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,		-0.5f, -0.5f, 0.0f,		0.5f, -0.5f, 0.0f,
	};
	float normals[] = {  
		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, 1.0f
	};
	float texCoords[] = {
		1.0f, 1.0f,		0.0f, 1.0f,		0.0f, 0.0f,
		1.0f, 1.0f,		0.0f, 0.0f,		1.0f, 0.0f
	};

	Primitive_SetupGL_Helper_Array(
		sVAO, sPosVBO, sNorVBO, sTexVBO,
        positions, normals, texCoords,
        sizeof(positions), sizeof(normals), sizeof(texCoords)
	);
}

Primitive::Primitive() {
	ShaderManager::Load("Primitive");
	pShader = ShaderManager::Get("Primitive");
	
	ResourceManager::LoadTexture("blank.png");
    pTexture = ResourceManager::GetTexture("blank.png");
}

void Primitive::SetTexture(const std::string& name) {
	ResourceManager::LoadTexture(name);
	pTexture = ResourceManager::GetTexture(name);
}

void Primitive::SetupGL() {
	Plane::SetupGL();
	Cube::SetupGL();
	Cylinder::SetupGL();
	Cone::SetupGL();
}

void Cylinder::OnDraw(Camera& cam) {
	if (!AllowDraw) return;
	pTexture->Bind();
	pShader->Use();
	pShader->SetMat4("uModelMat", transform.GetModelMatrix());
	pShader->SetMat4("uViewMat", cam.GetViewMat());
	pShader->SetMat4("uProjMat", cam.GetProjMat());
	glBindVertexArray(sVAO);
	glDrawArrays(GL_TRIANGLES, 0, sCount);
	//glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	pShader->Unuse();
}

void Cylinder::SetupGL() {

/*
b   a
0---1
| \ |
3---2
*/
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> texCoords;


	// side face
	for (int i = 0; i < SEGMENT; i++) {

		float angleA = AngleStep * (i + 1);	// radian
        float angleB = AngleStep * i;		// radian
		//Logger::Debug("{} {}", glm::degrees(angleA), glm::degrees(angleB));
	
		float ax = glm::cos(angleA);
		float az = glm::sin(angleA);
		float bx = glm::cos(angleB);
		float bz = glm::sin(angleB);

		// 調整頂點順序以符合逆時針
		positions.emplace_back(bx, H, bz);  // 0
		positions.emplace_back(ax, H, az);   // 1
		positions.emplace_back(ax, -H, az);   // 2
		positions.emplace_back(bx, H, bz);   // 0
		positions.emplace_back(ax, -H, az);   // 2
		positions.emplace_back(bx, -H, bz);   // 3

		normals.emplace_back(bx, 0, bz);   
		normals.emplace_back(ax, 0, az);
		normals.emplace_back(ax, 0, az);  
		normals.emplace_back(bx, 0, bz);
		normals.emplace_back(ax, 0, az);
		normals.emplace_back(bx, 0, bz);

		// 紋理座標計算（每90度重複）
		auto wrapUV = [](float angle) {
			constexpr float segmentAngle = glm::pi<float>();  // 90度弧度值
			const float invSegmentAngle = 1.0f / segmentAngle;
			return fmod(angle, segmentAngle) * invSegmentAngle;
		};

		texCoords.emplace_back(angleB / _2PI, 1.0f);      // 0 
		texCoords.emplace_back(angleA / _2PI, 1.0f);      // 1
		texCoords.emplace_back(angleA / _2PI, 0.0f);      // 2
		texCoords.emplace_back(angleB / _2PI, 1.0f);      // 0
		texCoords.emplace_back(angleA / _2PI, 0.0f);      // 2
		texCoords.emplace_back(angleB / _2PI, 0.0f);      // 3
	} 

	// top face
/*
*	      O
	    /   \     
	b+---------+a
*/
	for (int i = 0; i < SEGMENT; i++) {
		float ax = glm::cos(AngleStep * (i + 1));
		float az = glm::sin(AngleStep * (i + 1));
		float bx = glm::cos(AngleStep * i);
		float bz = glm::sin(AngleStep * i);

		positions.emplace_back(0, H, 0);	// OAB
		positions.emplace_back(ax, H, az);
		positions.emplace_back(bx, H, bz);

		normals.emplace_back(0, 1, 0);
		normals.emplace_back(0, 1, 0);
		normals.emplace_back(0, 1, 0);

		texCoords.emplace_back(0.5f, 0.5f);
		texCoords.emplace_back(-ax * 0.5f + 0.5f, -az * 0.5f + 0.5f);
		texCoords.emplace_back(-bx * 0.5f + 0.5f, -bz * 0.5f + 0.5f);
	}
	// botton face
	for (int i = 0; i < SEGMENT; i++) {
		float ax = glm::cos(AngleStep * (i + 1));
		float az = glm::sin(AngleStep * (i + 1));
		float bx = glm::cos(AngleStep * i);
		float bz = glm::sin(AngleStep * i);
		// OBA
		positions.emplace_back(0, -H, 0);	
		positions.emplace_back(bx, -H, bz);
		positions.emplace_back(ax, -H, az);

		normals.emplace_back(0, -1, 0);
		normals.emplace_back(0, -1, 0);
		normals.emplace_back(0, -1, 0);

		texCoords.emplace_back(0.5f, 0.5f);
		texCoords.emplace_back(-bx * 0.5f + 0.5f, -bz * 0.5f + 0.5f);
		texCoords.emplace_back(-ax * 0.5f + 0.5f, -az * 0.5f + 0.5f);
	}

	sCount = positions.size();
	Primitive_SetupGL_Helper_Vector(
		sVAO, sPosVBO, sNorVBO, sTexVBO,
		positions, normals, texCoords
	);
}

void Cone::OnDraw(Camera& cam) {
	if (!AllowDraw) return;
	pTexture->Bind();
	pShader->Use();
	pShader->SetMat4("uModelMat", transform.GetModelMatrix());
	pShader->SetMat4("uViewMat", cam.GetViewMat());
	pShader->SetMat4("uProjMat", cam.GetProjMat());
	glBindVertexArray(sVAO);
	glDrawArrays(GL_TRIANGLES, 0, sCount);
	glBindVertexArray(0);
	pShader->Unuse();
}

void Cone::SetupGL() {

	/*
	b           a
	0+---------+1
	 |\        |
	 |  \      |
	 |    X    |
	 |      \  |
	 |        \|
	3+---------+2
	*/
	vector<vec3> positions;
	vector<vec3> normals;
	vector<vec2> texCoords;


	// side face
	for (int i = 0; i < SEGMENT; i++) {

		float angleA = AngleStep * (i + 1);	// radian
		float angleB = AngleStep * i;		// radian
		//Logger::Debug("{} {}", glm::degrees(angleA), glm::degrees(angleB));

		float ax = glm::cos(angleA);
		float az = glm::sin(angleA);
		float bx = glm::cos(angleB);
		float bz = glm::sin(angleB);

		positions.emplace_back(bx * TOP_R,	H,	bz * TOP_R);  // 0
		positions.emplace_back(ax * TOP_R,	H,	az * TOP_R);   // 1
		positions.emplace_back(ax * BOT_R, -H,	az * BOT_R);   // 2
		positions.emplace_back(bx * TOP_R,	H,	bz * TOP_R);   // 0
		positions.emplace_back(ax * BOT_R, -H,	az * BOT_R);   // 2
		positions.emplace_back(bx * BOT_R, -H,	bz * BOT_R);   // 3

		normals.emplace_back(bx, 0, bz);
		normals.emplace_back(ax, 0, az);
		normals.emplace_back(ax, 0, az);
		normals.emplace_back(bx, 0, bz);
		normals.emplace_back(ax, 0, az);
		normals.emplace_back(bx, 0, bz);

		// 紋理座標計算（每90度重複）
		auto wrapUV = [](float angle) {
			constexpr float segmentAngle = glm::pi<float>();  // 90度弧度值
			const float invSegmentAngle = 1.0f / segmentAngle;
			return fmod(angle, segmentAngle) * invSegmentAngle;
			};

		texCoords.emplace_back(angleB / _2PI, 1.0f);      // 0 // or (wrapUV(angleB)
		texCoords.emplace_back(angleA / _2PI, 1.0f);      // 1
		texCoords.emplace_back(angleA / _2PI, 0.0f);      // 2
		texCoords.emplace_back(angleB / _2PI, 1.0f);      // 0
		texCoords.emplace_back(angleA / _2PI, 0.0f);      // 2
		texCoords.emplace_back(angleB / _2PI, 0.0f);      // 3
	}



	// top face
/*
*	      O
		/   \
	b+---------+a
*/
	for (int i = 0; i < SEGMENT; i++) {
		float ax = glm::cos(AngleStep * (i + 1));
		float az = glm::sin(AngleStep * (i + 1));
		float bx = glm::cos(AngleStep * i);
		float bz = glm::sin(AngleStep * i);

		positions.emplace_back(0, H, 0);	// OAB
		positions.emplace_back(ax * TOP_R, H, az * TOP_R);
		positions.emplace_back(bx * TOP_R, H, bz * TOP_R);

		normals.emplace_back(0, 1, 0);
		normals.emplace_back(0, 1, 0);
		normals.emplace_back(0, 1, 0);

		texCoords.emplace_back(0.5f, 0.5f);
		texCoords.emplace_back(-ax * 0.5f + 0.5f, -az * 0.5f + 0.5f);
		texCoords.emplace_back(-bx * 0.5f + 0.5f, -bz * 0.5f + 0.5f);
	}
	// bottom face
	for (int i = 0; i < SEGMENT; i++) {
		float ax = glm::cos(AngleStep * (i + 1));
		float az = glm::sin(AngleStep * (i + 1));
		float bx = glm::cos(AngleStep * i);
		float bz = glm::sin(AngleStep * i);
		// OBA
		positions.emplace_back(0, -H, 0);
		positions.emplace_back(bx* BOT_R, -H, bz * BOT_R);
		positions.emplace_back(ax * BOT_R, -H, az * BOT_R);

		normals.emplace_back(0, -1, 0);
		normals.emplace_back(0, -1, 0);
		normals.emplace_back(0, -1, 0);

		texCoords.emplace_back(0.5f, 0.5f);
		texCoords.emplace_back(-bx * 0.5f + 0.5f, -bz * 0.5f + 0.5f);
		texCoords.emplace_back(-ax * 0.5f + 0.5f, -az * 0.5f + 0.5f);
	}

	sCount = positions.size();
	Primitive_SetupGL_Helper_Vector(
		sVAO, sPosVBO, sNorVBO, sTexVBO,
		positions, normals, texCoords
	);
}

//------------------


#pragma once
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "Core/Shader.h"
#include "Core/Transform.h"
#include "Core/Texture.h"
#include "Core/GameObject.h"

// forward declaration
struct Circle;
class Camera;
using GLuint = unsigned int;
using std::shared_ptr;
using std::string;

const float PI = 3.141592653589793;

// both cone and clinder have texture issue, see TreeBuilder solution
class Primitive : public GameObject {
public:
	Primitive();
	virtual ~Primitive() = default;
public:
	Transform transform;
protected:
	shared_ptr<Texture> pTexture;
	shared_ptr<Shader> pShader;
public:
	void SetTexture(const string& name);
public:
	static void SetupGL();
};


class Plane :public Primitive {
public:
	Plane() = default;
public:
	void OnDraw(Camera& cam) override;
public:
	static void SetupGL();
private:
	inline static GLuint sVAO = 0;
	inline static GLuint sPosVBO = 0;	// postiion
	inline static GLuint sNorVBO = 0;	// normal
	inline static GLuint sTexVBO = 0;	// texture
};

class Cylinder : public Primitive {
public:
	Cylinder() = default;
public:
	void OnDraw(Camera& cam) override;
	int count;
public:
	static void SetupGL();
private:
	inline static int sCount = 0;
	inline static GLuint sVAO = 0;
	inline static GLuint sPosVBO = 0;
    inline static GLuint sNorVBO = 0;
	inline static GLuint sTexVBO = 0;
private:
	inline static int SEGMENT = 12;
	inline static float H = 0.5f;
	inline static float _2PI = 2.0 * PI;
	inline static float AngleStep = PI * 2.0f / (float)SEGMENT;
};

class Cube : public Primitive {
public:

	Cube() = default;
public:
	void OnDraw(Camera& cam) override;
	void SetTexture(const std::string& name);
public:
	static void SetupGL();
private:
	inline static GLuint sVAO = 0;
	inline static GLuint sPosVBO = 0;	// position
	inline static GLuint sNorVBO = 0;	// normal
	inline static GLuint sTexVBO = 0;	// texture
};
class Cone : public Primitive {
public:
	Cone() = default;
public:
	void OnDraw(Camera& cam) override;
	int count;
public:
	static void SetupGL();
private:
	inline static int sCount = 0;
	inline static GLuint sVAO = 0;
	inline static GLuint sPosVBO = 0;
	inline static GLuint sNorVBO = 0;
	inline static GLuint sTexVBO = 0;
private:
	inline static int SEGMENT = 12;
	inline static float H = 0.5f;
	inline static float _2PI = 2.0 * PI;
	inline static float AngleStep = PI * 2.0f / (float)SEGMENT;
	inline static float TOP_R = 0.0f;	// top radius
	inline static float BOT_R = 2.0f;	// bottom radius
};






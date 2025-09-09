#pragma once
#include <glm/glm.hpp>
#include "Core/Camera.h"

using std::shared_ptr;
using glm::vec3;

class Shader;
struct Line3D;
struct Box3D;
struct Rect3D;
struct Ray;

class Brush3D {
public:
	const float DEFAULT_POINT_SIZE = 1;
	const float DEFAULT_LINE_WIDTH = 1;
	const float AXIS_LENGTH = 30.0f;
private:
	shared_ptr<Shader> pShader;
	GLuint mVAO{ 0 };
	GLuint mVBO{ 0 };
	GLuint mEBO{ 0 };
	vec3 mColor = vec3(1.0f, 0.5f, 0.0f); // orange color
	float mPointSize{ DEFAULT_POINT_SIZE };
	float mLineWidth{ DEFAULT_LINE_WIDTH };
private:	// opengl backend render function
	inline void drawline() const;
	inline void drawbox() const;
	inline void drawquad() const;
	inline void drawtriangle() const;
	inline void drawpoint() const;
	void SetupGL();
public:
	Brush3D();
	void DrawRay(const Ray& ray, float length = 10.0f);
	void DrawGrid(float unit = 1);
	void CleanGL();
	void DrawQuad(const vec3& a, const vec3& b, const vec3& c);
	void DrawAxis(vec3 origin = vec3(0), float axisLength = 30);
	void DrawTriangle(const vec3& a, const vec3& b, const vec3& c);
	void DrawLine(const vec3& a, const vec3& b);
	void DrawLine(const Line3D& line);
	void DrawPoint(const vec3& a);
	void DrawBox(const vec3& min, const vec3& max);
	void DrawBox(const Box3D& box);
	void DrawBox(const Rect3D& rect);
	void Begin(Camera& camera);
	void End();
	void SetPointSize(float size);
	void SetLineWidth(float width);
	void SetColor(const vec3 color);

private:
	vec3 mVertices[8] = {
		vec3(0),	// A 0
		vec3(0),	// B 1
		vec3(0),	// C 2
		vec3(0),	// D 3
		vec3(0),	// a 4
		vec3(0),	// b 5
		vec3(0),	// c 6
		vec3(0),	// d 7
	};
	GLuint mIndices[24] = {
		0,1,1,2,2,3,3,0,
		4,5,5,6,6,7,7,4,
		0,4,1,5,2,6,3,7
	};
};
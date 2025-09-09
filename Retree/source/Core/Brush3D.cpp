#include "pch.h"

#include "Core/Brush3D.h"
#include "Core/ShaderManager.h"
#include "Core/Shape3D.h"
#include "Core/Ray.h"
#include "Core/Color.h"
using namespace std;
using namespace glm;


// opengl backend render function
inline void Brush3D::drawline() const {
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * sizeof(vec3), mVertices);
	glDrawArrays(GL_LINES, 0, 2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

inline void Brush3D::drawbox() const {
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 8 * sizeof(vec3), mVertices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

inline void Brush3D::drawquad() const {
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(vec3), mVertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);	// check
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

inline void Brush3D::drawtriangle() const {
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(vec3), mVertices);
	glDrawArrays(GL_TRIANGLES, 0, 3);		// check
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

inline void Brush3D::drawpoint() const {
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3), mVertices);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Brush3D::SetupGL() {
	// ShaderManager::Load("Brush3D");
	pShader = ShaderManager::Get("Brush3D");

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &mEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices), mIndices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Brush3D::Brush3D() {
	SetupGL();
}

void Brush3D::DrawRay(const Ray& ray, float length) {
	mVertices[0] = ray.origin;
	mVertices[1] = ray.At(length);
	drawline();
}

void Brush3D::DrawGrid(float unit) {
	for (float i = -AXIS_LENGTH; i <= AXIS_LENGTH; i += unit) {
		DrawLine(vec3(i, 0, -AXIS_LENGTH), vec3(i, 0, AXIS_LENGTH));
	}
	for (float i = -AXIS_LENGTH; i <= AXIS_LENGTH; i += unit) {
		DrawLine(vec3(-AXIS_LENGTH, 0, i), vec3(AXIS_LENGTH, 0, i));
	}
}

void Brush3D::CleanGL() {

	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);

}

void Brush3D::DrawQuad(const vec3& a, const vec3& b, const vec3& c) {
	mVertices[0] = a;
	mVertices[1] = b;
	mVertices[2] = b + c - a;
	mVertices[3] = c;
	drawquad();
}



void Brush3D::DrawAxis(vec3 origin, float axisLength) {
	SetLineWidth(3);
	SetColor(Color::Red);
	DrawLine(origin + vec3(0), origin + vec3(axisLength, 0, 0));
	SetColor(Color::Green);
	DrawLine(origin + vec3(0), origin + vec3(0, axisLength, 0));
	SetColor(Color::Blue);
	DrawLine(origin + vec3(0), origin + vec3(0, 0, axisLength));
	SetLineWidth(DEFAULT_LINE_WIDTH);
}

void Brush3D::DrawTriangle(const vec3& a, const vec3& b, const vec3& c) {
	mVertices[0] = a;
	mVertices[1] = b;
	mVertices[2] = c;
	drawtriangle();
}

void Brush3D::DrawLine(const vec3& a, const vec3& b) {
	mVertices[0] = a;
	mVertices[1] = b;
	drawline();
}

void Brush3D::DrawLine(const Line3D& line) {
	mVertices[0] = line.start;
	mVertices[1] = line.end;
	drawline();
}

void Brush3D::DrawPoint(const vec3& a) {
	mVertices[0] = a;
	drawpoint();
}

void Brush3D::DrawBox(const vec3& min, const vec3& max) {
	mVertices[0] = vec3(min.x, min.y, min.z);
	mVertices[1] = vec3(max.x, min.y, min.z);
	mVertices[2] = vec3(max.x, max.y, min.z);
	mVertices[3] = vec3(min.x, max.y, min.z);
	mVertices[4] = vec3(min.x, min.y, max.z);
	mVertices[5] = vec3(max.x, min.y, max.z);
	mVertices[6] = vec3(max.x, max.y, max.z);
	mVertices[7] = vec3(min.x, max.y, max.z);
	drawbox();

}

void Brush3D::DrawBox(const Box3D& box) {
	DrawBox(box.min, box.max);
}

void Brush3D::DrawBox(const Rect3D& rect) {
	vec3 min(rect.x, rect.y, rect.z);
	vec3 max(rect.x + rect.width, rect.y + rect.height, rect.z + rect.depth);
	DrawBox(min, max);
	drawbox();
}

void Brush3D::Begin(Camera& camera) {
	pShader->Use();
	pShader->SetMat4("uViewMat", camera.GetViewMat());
	pShader->SetMat4("uProjMat", camera.GetProjMat());
}

void Brush3D::End() {
	pShader->Unuse();
}

void Brush3D::SetPointSize(float size) {
	mPointSize = size;
	glPointSize(size);
}

void Brush3D::SetLineWidth(float width) {
	mLineWidth = width;
	glLineWidth(width);
}

void Brush3D::SetColor(const vec3 color) {
	mColor = color;
	pShader->SetVec3("uColor", color);
}



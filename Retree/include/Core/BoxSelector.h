#pragma once
#include <memory>
#include "Core/ShaderManager.h"

struct Rect2D;
using GLuint = unsigned int;

class BoxSelector {
public:
	BoxSelector() = default;
	BoxSelector(int width, int height);
private:
	bool mIsBoxVisible = false;
	glm::vec2 mRectStart;
	glm::vec2 mRectEnd;
	std::shared_ptr<Shader> pShader;
	GLuint mVAO{ 0 };
	GLuint mVBO{ 0 };
	glm::mat4 mProjMat;

	int mWidth;
	int mHeight;

	float mVertices[8] = {
		0.0f, 0.0f,		// top-left
		1.0f, 0.0f,		// top-right
		1.0f, 1.0f,		// bottom-right
		0.0f, 1.0f,		// bottom-left
	};
private:
	void SetupGL();
public:
	void SetActive(bool isActive);
	void SetRectStart(const glm::vec2& start);
	void SetRectEnd(const glm::vec2& end);
	void SetScreenSize(int width, int height);
	Rect2D GetRect() const;
	void Update();
	void CleanGL();
	void OnDraw();
};

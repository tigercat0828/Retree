#pragma once
#include <memory>
#include <functional>
#include "Core/ShaderManager.h"
#include "Core/Renderer.h"

using GLuint = unsigned int;
using std::shared_ptr;
using std::function;

class Camera;
class PointCloud;

class PointCloudRenderer : public Renderer {
public:
	PointCloudRenderer();
	~PointCloudRenderer();
private:
	PointCloud* pCloud = nullptr;
	GLuint mVAO{ 0 };
	GLuint mPositionVBO{ 0 };
	GLuint mColorVBO{ 0 };
	float mPointSize{ 2 };
public:
	void SetPointSize(float size);
	void Attach(PointCloud& pointCloud);

public:
	void OnDraw(Camera& cam);
private:
	shared_ptr<Shader> pShader;
	void UploadBuffer() override;
	void UpdateBuffer() override;;
	void SetupGL() override;
	void ReleaseGL() override;
};
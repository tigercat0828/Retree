#include "pch.h"
#include "Retree/PointCloud.h"

#include "Core/Camera.h"
#include "Retree/PointCloudRenderer.h"
using glm::mat4;
using glm::vec3;

PointCloudRenderer::PointCloudRenderer() {
	pShader = ShaderManager::Get("PointCloud");
	SetupGL();
}

inline PointCloudRenderer::~PointCloudRenderer() {
	// if (pCloud) pCloud->RemoveObserver(mDirtyCallback);  crash
}

void PointCloudRenderer::OnDraw(Camera& cam) {
	if (!AllowDraw || !pCloud) return;

	if (mDirtyFlag) {
		UpdateBuffer();
        mDirtyFlag = false;
	}
	glPointSize(mPointSize);
	pShader->Use();
	pShader->SetMat4("uModelMat", pCloud->transfrom.GetModelMatrix());
	pShader->SetMat4("uViewMat", cam.GetViewMat());
	pShader->SetMat4("uProjMat", cam.GetProjMat());
	glBindVertexArray(mVAO);
	glDrawArrays(GL_POINTS, 0, pCloud->GetCount());
	glBindVertexArray(0);
	pShader->Unuse();
	glPointSize(1);
}

void PointCloudRenderer::Attach(PointCloud& pointCloud) {
	pCloud = &pointCloud;

	mDirtyCallback = std::make_shared<function<void()>>(
		[this]() {	this->MarkDirty(); }
	);

	pointCloud.AddObserver(mDirtyCallback);

	UploadBuffer();
}



void PointCloudRenderer::SetPointSize(float size) {
	mPointSize = size;
}

void PointCloudRenderer::SetupGL() {
	
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mPositionVBO);
	glGenBuffers(1, &mColorVBO);
	
	glBindVertexArray(mVAO);

	// 配置位置屬性 (location = 0)
	glBindBuffer(GL_ARRAY_BUFFER, mPositionVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	// 配置顏色屬性 (location = 1)
	glBindBuffer(GL_ARRAY_BUFFER, mColorVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PointCloudRenderer::UploadBuffer() {
	const auto& positions = pCloud->GetPositions();
	glBindBuffer(GL_ARRAY_BUFFER, mPositionVBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);

	const auto& colors = pCloud->GetColors();
	glBindBuffer(GL_ARRAY_BUFFER, mColorVBO);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void PointCloudRenderer::UpdateBuffer() {

	const auto& positions = pCloud->GetPositions();
	glBindBuffer(GL_ARRAY_BUFFER, mPositionVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) *positions.size(), positions.data());
	
	const auto& colors = pCloud->GetColors();
	glBindBuffer(GL_ARRAY_BUFFER, mColorVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * colors.size(), colors.data());

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PointCloudRenderer::ReleaseGL() {

	if (glIsBuffer(mPositionVBO)) {
		glDeleteBuffers(1, &mPositionVBO);
		mPositionVBO = 0;
	}
	if (glIsBuffer(mColorVBO)) {
		glDeleteBuffers(1, &mColorVBO);
        mColorVBO = 0;
	}
	if (glIsVertexArray(mVAO)) {
		glDeleteVertexArrays(1, &mVAO);
        mVAO = 0;
	}
}



#include "pch.h"
#include "Core/BoxSelector.h"
#include "Core/Color.h"
#include "Core/Shape2D.h"

using namespace glm;

BoxSelector::BoxSelector(int width, int height) :mWidth(width), mHeight(height) { SetupGL(); }

void BoxSelector::SetupGL() {
	pShader = ShaderManager::Get("Screen");
	mProjMat = glm::ortho(0.0f, (float)mWidth, (float)mHeight, 0.0f, -1.0f, 1.0f);

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glBindVertexArray(mVAO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BoxSelector::SetRectStart(const glm::vec2& start) {
	this->mRectStart = start;
	mRectEnd = start;
}

void BoxSelector::SetRectEnd(const glm::vec2& end) {
	this->mRectEnd= end;

}

void BoxSelector::SetScreenSize(int width, int height) {
	if(width == mWidth && height == mHeight)return;
	mWidth = width;
	mHeight = height;
	mProjMat = glm::ortho(0.0f, (float)mWidth, (float)mHeight, 0.0f, -1.0f, 1.0f);
}

Rect2D BoxSelector::GetRect() const {
	float x = min(mRectStart.x, mRectEnd.x);
	float y = min(mRectStart.y, mRectEnd.y);
	float w = abs(mRectStart.x - mRectEnd.x);
	float h = abs(mRectStart.y - mRectEnd.y);
	return Rect2D(x, y, w, h);
}

void BoxSelector::Update() {
	if (!mIsBoxVisible)return;
	mVertices[0] = mRectStart.x;
	mVertices[1] = mRectStart.y;
	mVertices[2] = mRectEnd.x;
	mVertices[3] = mRectStart.y;
	mVertices[4] = mRectEnd.x;
	mVertices[5] = mRectEnd.y;
	mVertices[6] = mRectStart.x;
	mVertices[7] = mRectEnd.y;
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mVertices), mVertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BoxSelector::CleanGL() {
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
}

void BoxSelector::OnDraw() {
	if(!mIsBoxVisible)return;

	pShader->Use();
	pShader->SetMat4("uProjMat", mProjMat);
	pShader->SetVec3("uColor", Color::Green);

    glBindVertexArray(mVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

 void BoxSelector::SetActive(bool isActive) {
	 mIsBoxVisible = isActive;
 }

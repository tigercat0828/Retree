#pragma once
#include "Core/Renderer.h"
#include "Core/ShaderManager.h"
#include "Core/Color.h"
#include "Core/Octree.h"
#include <memory>

using std::shared_ptr;
using GLuint = unsigned int;

template<typename T>
class OctreeRenderer : public Renderer {
public:
	OctreeRenderer() {
		SetupGL();
	}
	shared_ptr<Shader> pShader;
	Octree<T>* pOctree = nullptr;
	GLuint mVAO{0};
	GLuint mVBO{0};
	GLuint mEBO{0};
	
	vec3 mColor = Color::LightGreen;
	int mDrawCount{ 0 };

	void Attach(Octree<T>& octree) {
		pOctree = &octree;
		UploadBuffer();
	}
	void OnDraw(Camera& cam) override {
		if (!AllowDraw || !pOctree ) return;
		pShader->Use();
		pShader->SetMat4("uViewMat", cam.GetViewMat());
        pShader->SetMat4("uProjMat", cam.GetProjMat());
        pShader->SetVec3("uColor", mColor);

		glBindVertexArray(mVAO);
		glDrawElements(GL_LINES, mDrawCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		pShader->Unuse();
	}
	void SetupGL() override {
		pShader = ShaderManager::Get("Brush3D");
	

		glGenVertexArrays(1, &mVAO);
		glGenBuffers(1, &mVBO);
		glGenBuffers(1, &mEBO);

		glBindVertexArray(mVAO);

		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
		glEnableVertexAttribArray(0);

	
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

		glBindVertexArray(0);
	}
	void UpdateBuffer() override {
		auto [vertice, indices] = pOctree->GetGLBuffer();
		if (indices.size() > mDrawCount) {
			glBindBuffer(GL_ARRAY_BUFFER, mVBO);
			glBufferData(GL_ARRAY_BUFFER, vertice.size() * sizeof(vec3), vertice.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		}
		else {
			// Partial update (reuse existing buffer)
			glBindBuffer(GL_ARRAY_BUFFER, mVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertice.size() * sizeof(vec3), vertice.data());

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), indices.data());
		}
		mDrawCount = indices.size();
	}
	void UploadBuffer() override {
		auto [vertice, indices] = pOctree->GetGLBuffer();
		mDrawCount = indices.size();
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferData(GL_ARRAY_BUFFER, vertice.size() * sizeof(vec3), vertice.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		
	}
	void ReleaseGL() override {
		if (glIsBuffer(mVBO)) {
			glDeleteBuffers(1, &mVBO);
			mVBO = 0;
		}
		// delete EBO
		if (glIsBuffer(mEBO)) {
			glDeleteBuffers(1, &mEBO);
			mEBO = 0;
		}
		if (glIsVertexArray(mVAO)) {
			glDeleteVertexArrays(1, &mVAO);
			mVAO = 0;
		}
	}
};
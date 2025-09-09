#pragma once
#include <memory>
#include <functional>
#include "Core/ShaderManager.h"
#include "Core/Renderer.h"

using GLuint = unsigned int;
using std::shared_ptr;
using std::function;
class Tree;

class TreeSkeletonRenderer :  public Renderer {
public:
	TreeSkeletonRenderer();
private:
	Tree* mTree = nullptr;
	GLuint mNodeVAO{ 0 };	// common node
	GLuint mNodeVBO{ 0 };
	GLuint mLeafVAO{ 0 };
	GLuint mLeafVBO{ 0 };
	GLuint mTrunkVAO{ 0 };
    GLuint mTrunkVBO{ 0 };
	GLuint mBranchVAO{ 0 };
    GLuint mBranchVBO{ 0 };

	GLuint mEdgeVAO{ 0 };
    GLuint mEdgeVBO{ 0 };

	shared_ptr<Shader> pShader;
public:
	void OnDraw(Camera& camera) override;

public:
	void Attach(Tree* tree);
private:
	void SetupGL() override ;
	void UpdateBuffer() override;
	void ReleaseGL() override;
	void UploadBuffer() override; 
};
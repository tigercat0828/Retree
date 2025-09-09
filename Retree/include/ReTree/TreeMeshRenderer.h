#pragma once
#include <memory>
#include <vector>
#include "Core/Renderer.h"
#include "Core/AssimpModel.h"
#include "Retree/TreeMeshBuilder.h"
using GLuint = unsigned int;
using std::string;
using std::shared_ptr;
using std::vector;
using glm::mat4;

// forward declaration
class Camera;
class Tree;
class Shader;
class Texture;
struct Line3D;
struct BranchMesh;


class TreeMeshRenderer : public Renderer { 
public:
	TreeMeshRenderer();
    bool AllowDrawBranch{ true };
	bool AllowDrawLeave{ true };
	bool EnableWindEffect{ true };
private:
	Tree* mTree = nullptr;
	// shaders
	shared_ptr<Shader> pLeafShader_Winded;
	shared_ptr<Shader> pBranchShader_Winded;
	shared_ptr<Shader> pLeafShader;
	shared_ptr<Shader> pBranchShader;

	// leave instance
	shared_ptr<AssimpModel> pLeafModel;
	vector<mat4> mTransformMatrices;	// <-- instancing target
	vector<GLuint> mInstancedVAOs;
	GLuint mLeaveVBO{ 0 };
	
	size_t mLeaveCount{ 0 };
	float mLeaveSize{ 0.1f };
	vector<Line3D> mLeaveLines;	// (leaf.parent to leaf)

	// Branch
	GLuint BranchVAO{ 0 };
	GLuint BranchPosVBO{ 0 };
	GLuint BranchNorVBO{ 0 };
	GLuint BranchTexVBO{ 0 };
	shared_ptr<Texture> pBranchTexture;
	size_t BranchMeshVertexCount{ 0 };
	BranchMesh mBrancheMesh;


public:

	void SetTreeStyle(const string& branchTex, const string& leafPrefab, float size);
	void OnDraw(Camera& cam) override;
	void SetLeafPrefab(const string& filename);
	void SetBranchTexture(const string& filename = "");
	void SetLeaveSize(float size);
	
private:
	void DrawBranches(Camera& cam);
	void DrawLeaves(Camera& cam);
	
	void UpLoadLeaveMeshBuffer(const vector<Line3D>& leaveEdges);
	void UploadBranchMeshBuffer(const BranchMesh& bGroup);
	void CloneInstancedVAOsFromModel();
	
	// ³z¹L Renderer Ä~©Ó
	void SetupGL() override;
	void ReleaseGL() override;
	void UpdateBuffer() override;
	void UploadBuffer() override;


public:
	void Attach(Tree* tree);
};
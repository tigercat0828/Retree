#include "pch.h"
#include "Retree/TreeMeshRenderer.h"

#include "Core/Window.h"
#include "Core/ShaderManager.h"
#include "Core/ResourceManager.h"
#include "Core/Camera.h"
#include "Core/Shape3D.h"
#include "Core/Transform.h"
#include "Retree/Tree.h"
#include "Retree/TreeMeshBuilder.h"
using std::vector;
using glm::vec3;
using glm::vec2;

TreeMeshRenderer::TreeMeshRenderer() {
	SetupGL();
}

void TreeMeshRenderer::SetLeafPrefab(const std::string& filename) {
	ResourceManager::LoadModel(filename);
	pLeafModel = ResourceManager::GetModel(filename);
}

void TreeMeshRenderer::SetBranchTexture(const std::string& filename) {
	ResourceManager::LoadTexture(filename);
	pBranchTexture = ResourceManager::GetTexture(filename);
}

void TreeMeshRenderer::SetTreeStyle(const std::string& branchTex, const std::string& leafPrefab, float size) {
	mLeaveSize = size;
	SetBranchTexture(branchTex);
	SetLeafPrefab(leafPrefab);
}

void TreeMeshRenderer::SetLeaveSize(float size) {
	mLeaveSize = size;
}

void TreeMeshRenderer::UploadBranchMeshBuffer(const BranchMesh& bGroup) {
	// upload buffer
	glBindBuffer(GL_ARRAY_BUFFER, BranchPosVBO);
	glBufferData(GL_ARRAY_BUFFER, bGroup.positions.size() * sizeof(vec3), bGroup.positions.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, BranchNorVBO);
	glBufferData(GL_ARRAY_BUFFER, bGroup.normals.size() * sizeof(vec3), bGroup.normals.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, BranchTexVBO);
	glBufferData(GL_ARRAY_BUFFER, bGroup.texCoords.size() * sizeof(vec2), bGroup.texCoords.data(), GL_STATIC_DRAW);

	BranchMeshVertexCount = bGroup.positions.size();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TreeMeshRenderer::CloneInstancedVAOsFromModel() {

	mInstancedVAOs.clear();

	if (!pLeafModel) return;

	for (const auto& mesh : pLeafModel->meshes) {
		GLuint newVAO;
		glGenVertexArrays(1, &newVAO);
		glBindVertexArray(newVAO);

		// === Bind shared mesh buffers ===
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);

		// Position (layout = 0)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex), (void*)0);
		glEnableVertexAttribArray(0);

		// Normal (layout = 1)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex), (void*)offsetof(AssimpVertex, Normal));
		glEnableVertexAttribArray(1);

		// TexCoord (layout = 2)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(AssimpVertex), (void*)offsetof(AssimpVertex, TexCoords));
		glEnableVertexAttribArray(2);

		// === Bind instance matrix buffer (mat4 = 4 vec4s) ===
		glBindBuffer(GL_ARRAY_BUFFER, mLeaveVBO);
		for (int i = 0; i < 4; ++i) {
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
			glVertexAttribDivisor(3 + i, 1);
		}

		glBindVertexArray(0);
		mInstancedVAOs.push_back(newVAO);
	}
}

void TreeMeshRenderer::UpLoadLeaveMeshBuffer(const vector<Line3D>& leaveEdges) {
	mLeaveCount = leaveEdges.size();
	mTransformMatrices.clear();

	for (const auto& leaveLine : leaveEdges) {
		Transform transform;
		// transform.position = leaveLine.start + mTree->transfrom.position;	// do here
		transform.position = leaveLine.end + mTree->transfrom.position;	// do here
		transform.scale = vec3(mLeaveSize);
		transform.SetForward(leaveLine.GetDirection());

		mTransformMatrices.push_back( transform.GetModelMatrix());
	}
	// setup instancing 
	
	glGenBuffers(1, &mLeaveVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mLeaveVBO);
	glBufferData(GL_ARRAY_BUFFER, mLeaveCount * sizeof(glm::mat4), &mTransformMatrices[0], GL_STATIC_DRAW);

	CloneInstancedVAOsFromModel();
}


void TreeMeshRenderer::SetupGL() {

	pBranchShader_Winded = ShaderManager::Get("Retree/windedBranch");
	pLeafShader_Winded = ShaderManager::Get("Retree/windedLeaf");

	pBranchShader = ShaderManager::Get("Primitive");
	pLeafShader = ShaderManager::Get("InstancedModel");

	ResourceManager::LoadTexture("branch2.jpg");
	pBranchTexture = ResourceManager::GetTexture("branch2.jpg");

	glGenVertexArrays(1, &BranchVAO);
	glGenBuffers(1, &BranchPosVBO);
	glGenBuffers(1, &BranchNorVBO);
	glGenBuffers(1, &BranchTexVBO);

	glBindVertexArray(BranchVAO);
	glBindBuffer(GL_ARRAY_BUFFER, BranchPosVBO);		// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, BranchNorVBO);		// normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, BranchTexVBO);		// texture coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
	glEnableVertexAttribArray(2);
}

void TreeMeshRenderer::ReleaseGL() {

	if (glIsBuffer(BranchPosVBO)) {
		glDeleteBuffers(1, &BranchPosVBO);
		BranchPosVBO = 0;
	}
	if (glIsBuffer(BranchNorVBO)) {
		glDeleteBuffers(1, &BranchNorVBO);
		BranchNorVBO = 0;
	}
	if (glIsBuffer(BranchTexVBO)) {
		glDeleteBuffers(1, &BranchTexVBO);
		BranchTexVBO = 0;
	}

	if (glIsVertexArray(BranchVAO)) {
		glDeleteVertexArrays(1, &BranchVAO);
		BranchVAO = 0;
	}
}

void TreeMeshRenderer::UpdateBuffer() {
	if (!mTree) return;

	// Re-generate branch and leaf data
	mBrancheMesh = TreeMeshBuilder::GenerateBranches(*mTree, 8);
	mLeaveLines = mTree->GetLeafEdges();

	// Clear old leaf buffer and VAOs
	if (glIsBuffer(mLeaveVBO)) {
		glDeleteBuffers(1, &mLeaveVBO);
		mLeaveVBO = 0;
	}

	for (GLuint vao : mInstancedVAOs) {
		if (glIsVertexArray(vao)) {
			glDeleteVertexArrays(1, &vao);
		}
	}
	mInstancedVAOs.clear();

	// Re-upload both branch and leaf data
	UpLoadLeaveMeshBuffer(mLeaveLines);
	UploadBranchMeshBuffer(mBrancheMesh);

}

void TreeMeshRenderer::UploadBuffer() {
	
	mBrancheMesh = TreeMeshBuilder::GenerateBranches(*mTree, 8);
	mLeaveLines = mTree->GetLeafEdges();
	
	UpLoadLeaveMeshBuffer(mLeaveLines);
	UploadBranchMeshBuffer(mBrancheMesh);
}

void TreeMeshRenderer::Attach(Tree* tree) {
	mTree = tree;

	/// not implement;
	mDirtyCallback = std::make_shared<function<void()>>(
		[this]() { this->MarkDirty(); }
	);
	mTree->AddObserver(mDirtyCallback);

	UploadBuffer();	// setup branch mesh and leaf mesh
}

void TreeMeshRenderer::OnDraw(Camera& cam)  {

	if (!AllowDraw || !mTree) return;

	if (mDirtyFlag) {
		UpdateBuffer();
		mDirtyFlag = false;
	}
	
	if (AllowDrawBranch) DrawBranches(cam);
	if (AllowDrawLeave) DrawLeaves(cam);
}


void TreeMeshRenderer::DrawBranches(Camera& cam) {
	pBranchTexture->Bind();

	if (EnableWindEffect) {
		pBranchShader_Winded->Use();
		// wind effect
		pBranchShader_Winded->SetFloat("uTime", Window::Instance().GetTime());
		pBranchShader_Winded->SetFloat("uWindStrength", 0.2f);
		pBranchShader_Winded->SetVec3("uWindDir", vec3(1, 0, 0));
		// ====================
		pBranchShader_Winded->SetMat4("uModelMat", mTree->transfrom.GetModelMatrix());
		pBranchShader_Winded->SetMat4("uViewMat", cam.GetViewMat());
		pBranchShader_Winded->SetMat4("uProjMat", cam.GetProjMat());
		glBindVertexArray(BranchVAO);
		glDrawArrays(GL_TRIANGLES, 0, BranchMeshVertexCount);
		glBindVertexArray(0);
		pBranchShader_Winded->Unuse();
	}
	else {
        pBranchShader->Use();
		pBranchShader->SetMat4("uModelMat", mTree->transfrom.GetModelMatrix());
		pBranchShader->SetMat4("uViewMat", cam.GetViewMat());
		pBranchShader->SetMat4("uProjMat", cam.GetProjMat());
		glBindVertexArray(BranchVAO);
		glDrawArrays(GL_TRIANGLES, 0, BranchMeshVertexCount);
		glBindVertexArray(0);
		pBranchShader->Unuse();
	}
}

void TreeMeshRenderer::DrawLeaves(Camera& cam) {
	
	
	if (EnableWindEffect) {
		pLeafShader_Winded->Use();
		// wind effect
		pLeafShader_Winded->SetFloat("uTime", Window::Instance().GetTime());
		pLeafShader_Winded->SetFloat("uWindStrength", 0.2f);
		pLeafShader_Winded->SetVec3("uWindDir", vec3(1, 0, 0));
		// ====================
		pLeafShader_Winded->SetMat4("uProjMat", cam.GetProjMat());
		pLeafShader_Winded->SetMat4("uViewMat", cam.GetViewMat());
		pLeafShader_Winded->SetInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pLeafModel->textures_loaded[0].id);  // note: we also made the textures_loaded vector public (instead of private) from the model class.
		for (unsigned int i = 0; i < pLeafModel->meshes.size(); i++) {

			glBindVertexArray(mInstancedVAOs[i]);
			glDrawElementsInstanced(
				GL_TRIANGLES,
				static_cast<GLsizei>(pLeafModel->meshes[i].indices.size()),
				GL_UNSIGNED_INT,
				0,
				mLeaveCount
			);

		}
		pLeafShader_Winded->Unuse();
	}
	else {
		pLeafShader->Use();
		pLeafShader->SetMat4("uProjMat", cam.GetProjMat());
		pLeafShader->SetMat4("uViewMat", cam.GetViewMat());
		pLeafShader->SetInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pLeafModel->textures_loaded[0].id); // note: we also made the textures_loaded vector public (instead of private) from the model class.
		for (unsigned int i = 0; i < pLeafModel->meshes.size(); i++) {

			glBindVertexArray(mInstancedVAOs[i]);
			glDrawElementsInstanced(
				GL_TRIANGLES,
				static_cast<GLsizei>(pLeafModel->meshes[i].indices.size()),
				GL_UNSIGNED_INT,
				0,
				mLeaveCount
			);
		}
		pLeafShader_Winded->Unuse();
	}
}
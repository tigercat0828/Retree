#include "pch.h"
#include "Core/Camera.h"
#include "Retree/Tree.h"
#include "Retree/TreeSkeletonRenderer.h"
using glm::mat4;
TreeSkeletonRenderer::TreeSkeletonRenderer() {

	SetupGL();
}
void TreeSkeletonRenderer::OnDraw(Camera& camera) {
	if (!AllowDraw || !mTree) return;

	if (mDirtyFlag) {
		UploadBuffer();
		mDirtyFlag = false;
	}

    pShader->Use();
	pShader->SetMat4("uModelMat", mTree->transfrom.GetModelMatrix());
	pShader->SetMat4("uViewMat", camera.GetViewMat());
	pShader->SetMat4("uProjMat", camera.GetProjMat());
	
	
	glBindVertexArray(mLeafVAO);
	// draw edges 
	glPointSize(2);	//3
	pShader->SetVec3("uColor", Color::Green);
	glDrawArrays(GL_POINTS, 0, mTree->GetLeafCount());

	glBindVertexArray(mNodeVAO);
	glPointSize(1);
	pShader->SetVec3("uColor", vec3(0.6f));
    glDrawArrays(GL_POINTS, 0, mTree->GetNodeCount());

	glBindVertexArray(mBranchVAO);
	glPointSize(1);	//3
	pShader->SetVec3("uColor", Color::Orange);
	glDrawArrays(GL_POINTS, 0, mTree->GetBranchCount());

	glPointSize(1);

	glLineWidth(1);	// 2
	glBindVertexArray(mEdgeVAO);
	pShader->SetVec3("uColor", Color::Gray);
    glDrawArrays(GL_LINES, 0, mTree->GetEdgeCount());
	glBindVertexArray(0);

	glLineWidth(2);
	glBindVertexArray(mTrunkVAO);
	pShader->SetVec3("uColor", Color::Red);
	glDrawArrays(GL_LINES, 0, mTree->GetTrunkEdgeCount());
	glLineWidth(1);

	// ========================================

	//glBindVertexArray(mLeafVAO);
	//// draw edges 
	//glPointSize(1);	//3
	//pShader->SetVec3("uColor", Color::Orange);
	//glDrawArrays(GL_POINTS, 0, mTree->GetLeafCount());

	//glBindVertexArray(mNodeVAO);
	//glPointSize(1);
	//pShader->SetVec3("uColor", Color::Orange);
	//glDrawArrays(GL_POINTS, 0, mTree->GetNodeCount());

	//glBindVertexArray(mBranchVAO);
	//glPointSize(1);	//3
	//pShader->SetVec3("uColor", Color::Orange);
	//glDrawArrays(GL_POINTS, 0, mTree->GetBranchCount());

	//glPointSize(1);

	//glLineWidth(3);	// 2
	//glBindVertexArray(mEdgeVAO);
	//pShader->SetVec3("uColor", Color::Gray);
	//glDrawArrays(GL_LINES, 0, mTree->GetEdgeCount());
	//glBindVertexArray(0);

	//glLineWidth(3);
	//glBindVertexArray(mTrunkVAO);
	//pShader->SetVec3("uColor", Color::Gray);
	//glDrawArrays(GL_LINES, 0, mTree->GetTrunkEdgeCount());
	//glLineWidth(1);

}


void TreeSkeletonRenderer::Attach(Tree* tree) {
	
	mTree = tree;
	mDirtyCallback = std::make_shared<function<void()>>(
		[this]() {  this->MarkDirty(); }	
	);
    mTree->AddObserver(mDirtyCallback);
	UploadBuffer();
}


void TreeSkeletonRenderer::SetupGL() {

	pShader = ShaderManager::Get("Retree/skeleton");
	glGenVertexArrays(1, &mLeafVAO);
	glGenVertexArrays(1, &mNodeVAO);
	glGenVertexArrays(1, &mBranchVAO);
	glGenVertexArrays(1, &mTrunkVAO);
	glGenVertexArrays(1, &mEdgeVAO);


	glGenBuffers(1, &mLeafVBO);
    glGenBuffers(1, &mNodeVBO);
    glGenBuffers(1, &mBranchVBO);
    glGenBuffers(1, &mTrunkVBO);
    glGenBuffers(1, &mEdgeVBO);

	glBindVertexArray(mNodeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mNodeVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(mLeafVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mLeafVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(mBranchVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mBranchVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(mTrunkVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mTrunkVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(mEdgeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mEdgeVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void TreeSkeletonRenderer::UpdateBuffer() {
	
	// Upload common node positions
	const auto& nodes = mTree->GetCommonNodePosition();
	glBindBuffer(GL_ARRAY_BUFFER, mNodeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * nodes.size(), nodes.data(), GL_DYNAMIC_DRAW);

	// Upload leaf node positions
	const auto& leaves = mTree->GetLeafNodePosition();
	glBindBuffer(GL_ARRAY_BUFFER, mLeafVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * leaves.size(), leaves.data(), GL_DYNAMIC_DRAW);

	// Upload branch node positions
	const auto& branches = mTree->GetBranchNodePosition();
	glBindBuffer(GL_ARRAY_BUFFER, mBranchVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * branches.size(), branches.data(), GL_DYNAMIC_DRAW);

	// Upload trunk edge positions
	const auto& trunks = mTree->GetTrunkEdgeList();
	glBindBuffer(GL_ARRAY_BUFFER, mTrunkVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * trunks.size(), trunks.data(), GL_DYNAMIC_DRAW);

	// Upload edge positions
	const auto& edges = mTree->GetEdgeList();
	glBindBuffer(GL_ARRAY_BUFFER, mEdgeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * edges.size(), edges.data(), GL_DYNAMIC_DRAW);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TreeSkeletonRenderer::ReleaseGL() {
	if (glIsBuffer(mNodeVBO)) {
		glDeleteBuffers(1, &mNodeVBO);
		mNodeVBO = 0;
	}
	if (glIsBuffer(mNodeVAO)) {
		glDeleteVertexArrays(1, &mNodeVAO);
		mNodeVAO = 0;
	}
	if (glIsBuffer(mLeafVBO)) {
		glDeleteBuffers(1, &mLeafVBO);
		mLeafVBO = 0;
	}
	if (glIsBuffer(mLeafVAO)) {
		glDeleteVertexArrays(1, &mLeafVAO);
		mLeafVBO = 0;
	}
	if (glIsBuffer(mBranchVBO)) {
		glDeleteBuffers(1, &mBranchVBO);
		mBranchVBO = 0;
	}
	if (glIsBuffer(mBranchVAO)) {
		glDeleteVertexArrays(1, &mBranchVAO);
		mBranchVAO = 0;
	}

	if (glIsBuffer(mTrunkVBO)) {
		glDeleteBuffers(1, &mTrunkVBO);
		mTrunkVBO = 0;
	}
	if (glIsBuffer(mTrunkVAO)) {
		glDeleteVertexArrays(1, &mTrunkVAO);
		mTrunkVAO = 0;
	}

	if (glIsBuffer(mEdgeVBO)) {
		glDeleteBuffers(1, &mEdgeVBO);
		mEdgeVBO = 0;
	}
	if (glIsBuffer(mEdgeVAO)) {
		glDeleteVertexArrays(1, &mEdgeVAO);
		mEdgeVAO = 0;
	}
}

void TreeSkeletonRenderer::UploadBuffer() {
	const auto& leafPosition = mTree->GetLeafNodePosition();
	glBindBuffer(GL_ARRAY_BUFFER, mLeafVBO);
	glBufferData(GL_ARRAY_BUFFER, leafPosition.size() * sizeof(vec3), leafPosition.data(), GL_STATIC_DRAW);

	const auto& nodePosition = mTree->GetCommonNodePosition();
	glBindBuffer(GL_ARRAY_BUFFER, mNodeVBO);
	glBufferData(GL_ARRAY_BUFFER, nodePosition.size() * sizeof(vec3), nodePosition.data(), GL_STATIC_DRAW);	

	const auto& branchPosition = mTree->GetBranchNodePosition();
	glBindBuffer(GL_ARRAY_BUFFER, mBranchVBO);
	glBufferData(GL_ARRAY_BUFFER, branchPosition.size() * sizeof(vec3), branchPosition.data(), GL_STATIC_DRAW);

    const auto& trunkPosition = mTree->GetTrunkEdgeList();
    glBindBuffer(GL_ARRAY_BUFFER, mTrunkVBO);
    glBufferData(GL_ARRAY_BUFFER, trunkPosition.size() * sizeof(vec3), trunkPosition.data(), GL_STATIC_DRAW);
	
	const auto& edges = mTree->GetEdgeList();
	glBindBuffer(GL_ARRAY_BUFFER, mEdgeVBO);
	glBufferData(GL_ARRAY_BUFFER, edges.size() * sizeof(vec3), edges.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}




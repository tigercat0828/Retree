#include "pch.h"
#include "Prefabs/EndlessGrid.h"
#include "Core/ShaderManager.h"
#include "Core/Camera.h"

EndlessGrid::EndlessGrid() {
	pShader = ShaderManager::Get("EndlessGrid");
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
}

void EndlessGrid::OnDraw(Camera& camera) {
	if (!AllowDraw) return;
	pShader->Use();
	glBindVertexArray(mVAO);
	auto view = camera.GetViewMat();
    auto proj = camera.GetProjMat();
	pShader->SetMat4("gVP", proj* view);
	pShader->SetVec3("gCameraWorldPos", camera.GetPosition());
	pShader->SetFloat("gGridSize", GridSize);
	pShader->SetFloat("gGridCellSize", CellSize);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

#include "pch.h"
#include "Core/AssimpObject.h"

#include "Core/ShaderManager.h"
#include "Core/ResourceManager.h"
#include "Core/Camera.h"

AssimpObject::AssimpObject(const std::string& filename) {
	pShader = ShaderManager::Get("AssimpModel");
	ResourceManager::LoadModel(filename);
	pModel = ResourceManager::GetModel(filename);
}

AssimpObject::AssimpObject(std::shared_ptr<AssimpModel> model) {
    pShader = ShaderManager::Get("AssimpModel");
    pModel = model;
}

void AssimpObject::OnDraw(Camera& cam) const  {
	pShader->Use();
	pShader->SetMat4("model", tranform.GetModelMatrix());
	pShader->SetMat4("view", cam.GetViewMat());
	pShader->SetMat4("projection", cam.GetProjMat());
	pModel->Draw(*pShader);
}

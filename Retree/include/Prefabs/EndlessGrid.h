#pragma once
#include <memory>
#include "Core/GameObject.h"
#include "Core/Shader.h"
using std::shared_ptr;
class Camera;
class EndlessGrid : public GameObject {
public:
	EndlessGrid();
private:
	std::shared_ptr<Shader> pShader;
	GLuint mVAO;
public:		// setter-getter
	float CellSize = 1.0f;
	float GridSize = 50.0f;
public:		// method
	void OnDraw(Camera& camera) override;	// Draw at last 
};
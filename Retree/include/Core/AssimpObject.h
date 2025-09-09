#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "Core/Transform.h"

using std::shared_ptr;
using std::string;

class AssimpModel;
class Camera;
class Shader;
class AssimpObject {

public:
	shared_ptr<AssimpModel> pModel;
	shared_ptr<Shader> pShader;
	Transform tranform;

	AssimpObject() = default;
	AssimpObject(const string& filename);
	AssimpObject(shared_ptr<AssimpModel> model);
	void OnDraw(Camera& cam) const ;
};
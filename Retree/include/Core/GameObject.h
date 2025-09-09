#pragma once
#include <string>
#include "Core/Transform.h"

using std::string;
class Camera;

class GameObject {
public: 
	bool AllowUpdate = true;
	bool AllowDraw = true;
	string Name;
	Transform transfrom;
	GameObject(const string& name ="");
	virtual ~GameObject() = default;

	virtual void OnLoad() {}
	virtual void OnUpdate(float delta) {}
	virtual void OnDraw(Camera& camera) {}
	virtual void OnDraw() {}
};
#pragma once
#include <glm/glm.hpp>
#include "Core/Camera.h"
#include "Core/Ray.h"
using glm::vec2;

class MousePicker {
public:
	MousePicker() = default;
	MousePicker(Camera* camera);
private:
	Camera* camera = nullptr;
	Ray mRay;
public: // getter-setter
	void AttachCamera(Camera* cam);
	Ray GetRay()  const;
public:
	bool CastRay(const vec2& mousePos , const vec2& screenSize);
private:
	vec2 GetCoordinateNDC(int mouseX, int mouseY, int width, int height) const;
};
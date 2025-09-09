#include "pch.h"
#include "Core/MousePicker.h"
using namespace glm;

MousePicker::MousePicker(Camera* camera) {
	this->camera = camera;
}

void MousePicker::AttachCamera(Camera* cam) {
	camera = cam;
}

Ray MousePicker::GetRay() const {
	return mRay;
}

bool MousePicker::CastRay(const glm::vec2& mousePos, const glm::vec2& screenSize) {
	if (!camera) return false;

	// 邊界檢查
	if (mousePos.x < 0 || mousePos.x > screenSize.x || mousePos.y < 0 || mousePos.y > screenSize.y)
		return false;

	// 轉換為 NDC
	auto normalCoord = GetCoordinateNDC(mousePos.x, mousePos.y, screenSize.x, screenSize.y);
	auto clipCoord = vec4(normalCoord.x, normalCoord.y, -1.0f, 1.0f);

	// Clip Space -> View Space
	auto viewCoord = inverse(camera->GetProjMat()) * clipCoord;
	viewCoord = vec4(viewCoord.x, viewCoord.y, -1.0f, 0.0f); // w=0 表示方向向量

	// View Space -> World Space
	vec3 worldRay = normalize(vec3(inverse(camera->GetViewMat()) * viewCoord));

	mRay = Ray(camera->GetPosition(), worldRay);
	return true;
}


glm::vec2 MousePicker::GetCoordinateNDC(int mouseX, int mouseY, int width, int height) const {
	float x = (2.0f * mouseX) / width - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / height;
	return glm::vec2(x, y);
}

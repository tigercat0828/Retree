#include "pch.h"
#include "Core/Camera.h"
#include "Core/Shape3D.h"
using namespace glm;



Camera::Camera(const glm::vec3& lookAt, const glm::vec3& from) {
	
	LookAtFrom(lookAt, from);
	
}


// getter-setter
vec3 Camera::GetPosition() const { return mPosition; }

void Camera::SetPosition(const glm::vec3& position) {
    mPosition = position;
	mIsViewDirty = true;
}

float Camera::GetPitch() const { return mPitch; }

float Camera::GetYaw() const { return mYaw; }

float Camera::GetAzimuth() const { return mAzimuthDeg; }



float Camera::GetElevation() const { return mElevationDeg; }

CamState Camera::GetState() const {
	return CamState{ mPosition, mYaw, mPitch, mZoom, mAzimuthDeg, mElevationDeg };
}

void Camera::SetState(const CamState& state) {
	mPosition = state.position;
	mYaw = state.yaw;
	mPitch = state.pitch;
	mZoom = state.zoom;
    mAzimuthDeg = state.azimuth;
    mElevationDeg = state.elevation;
	UpdateVectors();
}

float Camera::GetMoveSpeed() const { return mMoveSpeed; }

float Camera::GetTurnSpeed() const { return mTurnSpeed; }

float Camera::GetZoom() const { return mZoom; }

float Camera::GetAspectRatio() const { return mAspectRatio; }

void Camera::GetViewport(int& width, int& height) const {
	width = mViewportWidth;
	height = mViewportHeight;
}

Camera::ProjectionMode Camera::GetProjectionMode() const { return mProjectionMode; }

const mat4& Camera::GetViewMat() {
	if (mIsViewDirty) {
		UpdateVectors();
	}
	return mViewMat;
}

const mat4& Camera::GetProjMat() {
	const float nearPlane = 0.1f;
	const float farPlane = 1000.0f;

	if (mIsProjDirty) {
		if (mProjectionMode == ProjectionMode::Perspective) {
			mProjMat = glm::perspective(glm::radians(mZoom), mAspectRatio, nearPlane, farPlane);
		}
		else {
			// 切換成正交投影
			// 正交投影
			float orthoSize = mZoom * mOrthoFactor; // 將透視投影的 zoom 角度轉換為正交大小
			float halfHeight = orthoSize;
			float halfWidth = halfHeight * mAspectRatio;
			mProjMat = ortho(
				-halfWidth, halfWidth,		// 左右邊界
				-halfHeight, halfHeight,    // 下上邊界
				nearPlane, farPlane			// 近平面和遠平面
			);
		}
		mIsProjDirty = false;
	}
	return mProjMat;
}

vec3 Camera::GetFocusPoint() const { return mFocusPoint; }

void Camera::SetOrbitRadius(float radius) {
	mOrbitRadius = glm::clamp(radius, 1.0f, 100.0f);
	mIsViewDirty = true;
}

void Camera::OnUpdate(float deltaTime) {
	ProcessInput(deltaTime);
}

void Camera::LookAtFrom(const vec3& target, const vec3& from) {

	if (mMotionMode == MotionMode::Free) {
		mPosition = from;
		vec3 direction = normalize(target - from);
		mYaw = glm::degrees(atan2(direction.z, direction.x));
		mPitch = glm::degrees(asin(direction.y));
		mFront = direction;
		mUp = vec3(0, 1, 0); // WORLD_UP
	}
	else if (mMotionMode == MotionMode::Orbit) {
		mFocusPoint = target;
		vec3 offset = target -from;
		mOrbitRadius = glm::length(offset);
		mAzimuthDeg = glm::degrees(atan2(offset.z, offset.x));
		mElevationDeg = glm::degrees(asin(offset.y / mOrbitRadius));

		// GetOrbitPosition();
	}

	mIsViewDirty = true;
	UpdateVectors();
}

void Camera::LookAt(const vec3& target) {
	LookAtFrom(target, mPosition);
	mIsViewDirty = true;
	UpdateVectors();
}

void Camera::SetAspectRatio(float width, float height) {
	mViewportWidth = width;
	mViewportHeight = height;
	mAspectRatio = width / height;
	mIsProjDirty = true;
}

void Camera::SetProjectionMode(ProjectionMode mode) {
	mProjectionMode = mode;
}

Camera::MotionMode Camera::GetMotionMode() const { return mMotionMode; }

void Camera::SetMotionMode(MotionMode newMode) {
	if (mMotionMode == newMode) return;

	if (newMode == MotionMode::Free) {
		RecalculateFreeFromOrbit();
	}
	else if (newMode == MotionMode::Orbit) {
		RecalculateOrbitFromFree();
	}
	UpdateVectors();
	mMotionMode = newMode;
	mIsViewDirty = true;
}

void Camera::SetFocusPoint(const vec3& target) {
	mFocusPoint = target;
	mIsViewDirty = true;
}

void Camera::FitBoxtToScreen(const Box3D& box) {
	vec3 size = box.GetSize();
	float radius = length(size) * 0.5f; // 半徑
	float distance = radius / tan(glm::radians(mZoom * 0.5f)); // 計算距離
	// 將攝影機移動到適合的距離
	vec3 center = box.GetCenter();
	vec3 direction = glm::normalize(mPosition - center);
	LookAtFrom(center, center + direction * distance);
}

void Camera::FocusBoxFromMaxCorner(const Box3D& box) {
	LookAtFrom(box.min, box.max);
}

void Camera::FitObjectToScreenToView(const Box3D& box) {
	vec3 size = box.GetSize();
	float radius = length(size) * 0.5f; // 半徑
	float distance = radius / tan(glm::radians(mZoom * 0.5f)); // 計算距離
	// 將攝影機移動到適合的距離
	vec3 center = box.GetCenter();
	vec3 direction = glm::normalize(mPosition - center);
	LookAtFrom(center, center + direction * distance);
}

void Camera::Revolve(float degree) {
	if (mMotionMode == MotionMode::Orbit)
		mAzimuthDeg += degree;
	mIsViewDirty = true;
}



void Camera::UpdateVectors() {
	if (mMotionMode == MotionMode::Free) {
		vec3 front;
		front.x = glm::cos(glm::radians(mYaw)) * glm::cos(glm::radians(mPitch));
		front.y = glm::sin(glm::radians(mPitch));
		front.z = glm::sin(glm::radians(mYaw)) * glm::cos(glm::radians(mPitch));
		this->mFront = normalize(front);

		mRight = glm::normalize(glm::cross(front, vec3(0, 1, 0)));
		mUp = glm::normalize(glm::cross(mRight, this->mFront));
		mViewMat = glm::lookAt(mPosition, mPosition + mFront, mUp);
	}
	else {
		float azimuthRad = glm::radians(mAzimuthDeg);
		float elevationRad = glm::radians(mElevationDeg);

		// 計算從焦點看出去的方向向量
		vec3 direction = GetOrbitPositionAndDirection();

		// calculate up
		vec3 worldUp = vec3(0, 1, 0);
		vec3 right = glm::normalize(glm::cross(worldUp, direction));
		vec3 up = glm::cross(direction, right);

		// 最終生成 ViewMatrix
		mViewMat = glm::lookAt(mPosition, mFocusPoint, up);

	}
	mIsViewDirty = false;
}

void Camera::Turn(float xOffset, float yOffset) {
	xOffset *= mTurnSpeed;
	yOffset *= mTurnSpeed;

	mYaw += xOffset;
	mPitch += yOffset;

	mPitch = glm::clamp(mPitch, -89.0f, 89.0f);

	mIsViewDirty = true;
}

void Camera::Zoom(float yOffset) {
	mZoom -= yOffset;
	if (mZoom < 1.0) mZoom = 1.0f;
	if (mZoom > 45.0) mZoom = 45.0f;
	mIsProjDirty = true;
}

void Camera::ProcessInput(float deltaTime) {
	if (mMotionMode == MotionMode::Free) 
		FreeMove(deltaTime);
	else 
		OrbitMove(deltaTime);
}

void Camera::FreeMove(float deltaTime) {
	float offset = mMoveSpeed * deltaTime;
	if (Input.GetKeyPressed(SDL_SCANCODE_W)) {
		vec3 movement = vec3(mFront.x, 0, mFront.z);
		mPosition += movement * offset;
		// mPosition += mFront * offset;
	}
	if (Input.GetKeyPressed(SDL_SCANCODE_S)) {
		vec3 movement = vec3(mFront.x, 0, mFront.z);
		mPosition -= movement * offset;
	}
	if (Input.GetKeyPressed(SDL_SCANCODE_A)) mPosition -= mRight * offset;
	if (Input.GetKeyPressed(SDL_SCANCODE_D)) mPosition += mRight * offset;
	if (Input.GetKeyPressed(SDL_SCANCODE_SPACE)) mPosition += vec3(0, 1, 0) * offset;
	if (Input.GetKeyPressed(SDL_SCANCODE_LSHIFT)) mPosition -= vec3(0, 1, 0) * offset;
	
	float scrollY = Input.GetMouseWheelY();
	Zoom(scrollY);
	vec2 mouseOffset = Input.GetMouseOffset();
	Turn(mouseOffset.x, -mouseOffset.y);
}

void Camera::OrbitMove(float deltaTime) {
	auto& input = InputManager::Instance();
	float rotateSpeed = mMoveSpeed * deltaTime * 8.0f;

	if (input.GetMouseButtonPressed(SDL_BUTTON_RIGHT)) {
		glm::vec2 offset = input.GetMouseOffset();
		mAzimuthDeg += offset.x * rotateSpeed;
		mElevationDeg -= offset.y * rotateSpeed;
		mElevationDeg = glm::clamp(mElevationDeg, -89.0f, 89.0f);
		mIsViewDirty = true;
	}

	if (Input.GetMouseButtonPressed(SDL_BUTTON_MIDDLE)) {
		glm::vec2 offset = input.GetMouseOffset(); // 滑鼠位移

		// 平移速度（可調整）
		float panSpeed = mOrbitRadius * 0.002f;  // 距離越遠，平移越快

		// 計算平移方向（以螢幕為基準：right / up）
		vec3 forward = glm::normalize(mFocusPoint - mPosition);
		vec3 right = glm::normalize(glm::cross(forward, vec3(0,1,0)));
		vec3 up = glm::normalize(glm::cross(right, forward));

		mFocusPoint -= right * offset.x * panSpeed;
		mFocusPoint += up * offset.y * panSpeed;
		mIsViewDirty = true;
	}

	float scroll = input.GetMouseWheelY();
	if (scroll != 0.0f) {
		mOrbitRadius -= scroll * deltaTime * 60.0f;
		mOrbitRadius = glm::clamp(mOrbitRadius, 0.1f, 100.0f);
		mIsViewDirty = true;
	}

}

void Camera::RecalculateFreeFromOrbit() {
	vec3 direction = glm::normalize(mFocusPoint - mPosition);
	mYaw = glm::degrees(atan2(direction.z, direction.x));
	mPitch = glm::degrees(asin(direction.y));

}

void Camera::RecalculateOrbitFromFree() {
	mFocusPoint = mPosition + mFront * mOrbitRadius;
	vec3 offset = mFocusPoint - mPosition;
	mOrbitRadius = glm::length(offset);

	mAzimuthDeg = glm::degrees(atan2(offset.z, offset.x));
	mElevationDeg = glm::degrees(asin(offset.y / mOrbitRadius));
	mElevationDeg = glm::clamp(mElevationDeg, -89.0f, 89.0f);
}

vec3 Camera::GetOrbitPositionAndDirection() {

	float azimuthRad = glm::radians(mAzimuthDeg);
	float elevationRad = glm::radians(mElevationDeg);

	vec3 direction;
	direction.x = glm::cos(elevationRad) * glm::cos(azimuthRad);
	direction.y = glm::sin(elevationRad);
	direction.z = glm::cos(elevationRad) * glm::sin(azimuthRad);
	// 相機位置 = 焦點 - 方向 * 距離
	mPosition = mFocusPoint - direction * mOrbitRadius;

	return direction;
}

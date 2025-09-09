#pragma once
#include <glm/glm.hpp>
#include "Core/InputManager.h"
struct Box3D;

using GLuint = unsigned int;
using glm::vec3;
using glm::mat4;

struct CamState {
	vec3 position;
	float yaw;
	float pitch;
	float zoom;
	float azimuth;
    float elevation;
};

class Camera {
public:
	enum class MotionMode { Free, Orbit };
	enum class ProjectionMode { Perspective, Orthographic };

public:	// constructor
	Camera() = default;
	Camera(const vec3& lookAt, const vec3& from);
private:
	//vec3 WORLD_UP = vec3(0, 1, 0);
private:	// properties
	float mYaw;				// Yaw is rotation around the Y axis
	float mPitch;			// Pitch is rotation around the X axis
	float mRoll{ 0 };		// Roll is rotation around the Z axis
	vec3 mPosition;

	vec3 mFront;
	vec3 mUp;
	vec3 mRight;

	float mViewportWidth;
	float mViewportHeight;
	float mAspectRatio{ 16.0f / 9.0f };

	float mMoveSpeed{ 3.0f };
	float mTurnSpeed{ 0.5f };
	float mZoom{ 30.0f };

	ProjectionMode mProjectionMode =  ProjectionMode::Perspective;
	//MotionMode mMotionMode = MotionMode::FirstPerson;
	MotionMode mMotionMode = MotionMode::Orbit;

	mat4 mViewMat;
    mat4 mProjMat;

	bool mIsViewDirty = true;
	bool mIsProjDirty = true;

	// for track ball
	vec3 mFocusPoint = vec3(0);
	float mAzimuthDeg{ 0.0f };	// horizontal
	float mElevationDeg{ 0.0 };		// vertical
	float mOrbitRadius = 5;

private:
	InputManager& Input = InputManager::Instance();
	float mOrthoFactor = 0.180f;
public: // getter-setter
	vec3 GetPosition() const;
	void SetPosition(const vec3& position);
	float GetPitch() const;
	float GetYaw() const;
	float GetAzimuth() const;
    float GetElevation() const;
	CamState GetState() const;
	void SetState(const CamState& state);
	float GetMoveSpeed() const;
	float GetTurnSpeed() const;
	float GetZoom() const;
	float GetAspectRatio() const;
	void GetViewport(int& width, int& height) const;
	ProjectionMode GetProjectionMode() const;
	const mat4& GetViewMat();
	const mat4& GetProjMat();
	vec3 GetFocusPoint() const;

	void SetAspectRatio(float width, float height);
	void SetProjectionMode(ProjectionMode mode);
	MotionMode GetMotionMode() const;
	void SetMotionMode(MotionMode mode);
	void SetFocusPoint(const vec3& target);
	void SetOrbitRadius(float radius);
public:
	void OnUpdate(float deltaTime);
public:
	void LookAtFrom(const vec3& target, const vec3& from);
	void LookAt(const vec3& target);
	void FitBoxtToScreen(const Box3D& box);
	void FocusBoxFromMaxCorner(const Box3D& box);
	void FitObjectToScreenToView(const Box3D& box);
	void Revolve(float degree);
private:
	void UpdateVectors();
	void Turn(float xOffset, float yOffset);
	void Zoom(float yOffset);
private:
	void ProcessInput(float deltaTime);
	void FreeMove(float deltaTime);
	void OrbitMove(float deltaTime);


	void RecalculateFreeFromOrbit();
	void RecalculateOrbitFromFree();

	vec3 GetOrbitPositionAndDirection();
};
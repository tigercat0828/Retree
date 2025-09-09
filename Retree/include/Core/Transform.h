#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
using glm::vec3;
using glm::quat;
using glm::mat4;

struct Transform {
public:
	vec3 position = vec3(0.0f);
	quat rotation = quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity
	vec3 scale =	vec3(1.0f);
	Transform(
		const vec3& position =				vec3(0.0f),
		const vec3& eulerRotationDegrees =	vec3(0.0f),
		const vec3& scale =					vec3(1.0f)
	);
    mat4 GetModelMatrix() const;
	// Rotation Setters/Getters
	void SetRotationEuler(const vec3& eulerDegrees);		// vec3 ➜ quat
	vec3 GetRotationEuler() const;							// quat ➜ vec3

	/// <summary>
	/// Accumulate rotation
	/// </summary>
	void AxisRotate(float angleDegree, const vec3& axis);

	/// <summary>
	/// reset the rotation
	/// </summary>
	void SetAxisRotate(float angleDegree, const vec3& axis);
	void SetForward(const vec3& forward);
	void Translate(const vec3& translation);
	void Translate(float x, float y, float z);
};


#include "pch.h"
#include "Core/Transform.h"


Transform::Transform(
    const vec3& position, 
    const vec3& rotation, 
    const vec3& scale
) :position(position), rotation(glm::quat(glm::radians(rotation))), scale(scale) {
}

mat4 Transform::GetModelMatrix() const {
    // SRT - scale > rotate(YXZ) > translate
    return 
        glm::translate(glm::mat4(1.0f), position) * 
        glm::toMat4(rotation) * 
        glm::scale(glm::mat4(1.0f), scale);
}

void Transform::SetRotationEuler(const glm::vec3& eulerDegrees) {
    rotation = glm::quat(glm::radians(eulerDegrees));
}

glm::vec3 Transform::GetRotationEuler() const {
    return glm::degrees(glm::eulerAngles(rotation));
}



/// <summary>
/// Accumulate rotation
/// </summary>
void Transform::AxisRotate(float angleDegree, const vec3& axis) {
    // Create a quaternion from axis-angle
    quat q = glm::angleAxis(glm::radians(angleDegree), glm::normalize(axis));
    // Combine it with the current rotation (apply q * rotation to rotate in local space)
    rotation = q * rotation;
}

/// <summary>
/// reset the rotation
/// </summary>
void Transform::SetAxisRotate(float angleDegree, const vec3& axis) {
    // Create a quaternion from axis-angle
    rotation = glm::angleAxis(glm::radians(angleDegree), glm::normalize(axis));
}

// Forward-setting utility
void Transform::SetForward(const glm::vec3& forward) {
    glm::vec3 fwd = glm::normalize(forward);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    if (glm::abs(glm::dot(fwd, up)) > 0.999f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f); // avoid degenerate case
    }

    glm::vec3 right = glm::normalize(glm::cross(up, fwd));
    up = glm::normalize(glm::cross(fwd, right));

    glm::mat3 rotMat(right, up, fwd);
    rotation = glm::quat_cast(rotMat);
}

void Transform::Translate(const vec3& translation) {
    position += translation;
}

void Transform::Translate(float x, float y, float z) {
    position += vec3(x, y, z);
}

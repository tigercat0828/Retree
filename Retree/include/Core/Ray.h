#pragma once
#include <glm/glm.hpp>
using glm::vec3;
struct Ray {
public:
	Ray();
	Ray(const vec3& origin, const vec3& direction);
	vec3 origin;
	vec3 direction;
	void SetDirection(const vec3& direction);
	vec3 At(float t) const;
};
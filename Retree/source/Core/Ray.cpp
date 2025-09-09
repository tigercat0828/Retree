#include "pch.h"
#include "Core/Ray.h"


Ray::Ray() : origin(vec3(0)), direction(vec3(1, 0, 0)) {}

Ray::Ray(const vec3& origin, const vec3& direction) : origin(origin), direction(glm::normalize(direction)) {}

void Ray::SetDirection(const vec3& direction) {
	this->direction = glm::normalize(direction);
}

vec3 Ray::At(float t) const {
	return origin + direction * t;
}

#include "pch.h"
#include "Core/Shape3D.h"

using std::pair;
Line3D::Line3D() : start(vec3(0)), end(vec3(0)) {}

Line3D::Line3D(const vec3& start, const vec3& end) : start(start), end(end) {}

Line3D::Line3D(const vec3& origin, float pitch, float yaw, float length) {
	start = origin;
	vec3 direction;
	direction.x = cos(yaw) * cos(pitch);
	direction.y = sin(pitch);
	direction.z = sin(yaw) * cos(pitch);
	end = start + direction * length;
}

pair<float, float> Line3D::GetPitchYawInRadian() const {
	vec3 dir = normalize(end - start);

	// Correct yaw calculation
	float yaw = std::atan2(dir.z, dir.x);

	// Pitch calculation remains the same
	float pitch = std::atan2(dir.y, sqrt(dir.x * dir.x + dir.z * dir.z));

	return { pitch, yaw };
}

float Line3D::GetLength() const {
	return glm::length(start - end);
}

vec3 Line3D::GetDirection() const {
	return vec3(glm::normalize(end - start));
}

float Line3D::Length() const {
	return glm::length(start - end);
}

ostream& operator<<(ostream& os, const Line3D& Line) {
	return os
		<< "(" << Line.start.x << ", " << Line.start.y << ", " << Line.start.z << ") => "
		<< "(" << Line.end.x << ", " << Line.end.y << ", " << Line.end.z << ")";
}


CylinderData::CylinderData() : bottom(vec3(0)), top(vec3(0)), radius(0), segment(0) {}

CylinderData::CylinderData(const vec3& bottom, const vec3& top, float radius, int segment) : bottom(bottom), top(top), radius(radius), segment(segment) {}

vector<vec3> CylinderData::GetVertexBuffer() {
	vector<vec3> vertices;
	return vertices;
}

Box3D::Box3D() : min(vec3(0)), max(vec3(0)){
	
}

Box3D::Box3D(const vec3& min, const vec3& max) : min(min), max(max) {}

vec3 Box3D::GetCenter() const { return (max + min) / 2.0f; }

vec3 Box3D::GetSize() const { return max - min; }

float Box3D::GetWidth() const { return max.x - min.x; }

float Box3D::GetHeight() const { return max.y - min.y; }

float Box3D::GetDepth() const { return max.z - min.z; }

bool Box3D::Contains(const vec3& vec) const {
	return
		vec.x >= min.x && vec.x <= max.x &&
		vec.y >= min.y && vec.y <= max.y &&
		vec.z >= min.z && vec.z <= max.z;
}

Rect3D Box3D::GetRect() const {
	return Rect3D(
		min.x, min.y, min.z, 
		max.x - min.x, max.y - min.y, max.z - min.z
	);
}

Rect3D::Rect3D(float x, float y, float z, float width, float height, float depth) : x(x), y(y), z(z), width(width), height(height), depth(depth) {}

Rect3D::Rect3D(const vec3& anchor, const vec3& size) {
	x = anchor.x;
	y = anchor.y;
	z = anchor.z;
	width = size.x;
	height = size.y;
	depth = size.z;
}

bool Rect3D::Contains(const vec3& position) const {
	return
		position.x >= x and position.x < x + width and
		position.y >= y and position.y < y + height and
		position.z >= z and position.z < z + depth;
}

bool Rect3D::Intersects(const Rect3D& other) const {
	return
		x < other.x + other.width and x + width > other.x and
		y < other.y + other.height and y + height > other.y and
		z < other.z + other.depth and z + depth > other.z;
}

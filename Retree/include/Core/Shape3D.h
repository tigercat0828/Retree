#pragma once
#include <vector>
#include <utility>
#include <iostream>
#include <glm/glm.hpp>

using glm::vec3;
using std::vector;
using std::ostream;

struct Rect3D {		// for octree boundary
public:
	float x{ 0 };
	float y{ 0 };
	float z{ 0 };
	float width{ 0 };
	float height{ 0 };
	float depth{ 0 };
	Rect3D() = default;
	Rect3D(float x, float y, float z, float width, float height, float depth);
	Rect3D(const vec3& anchor, const vec3& size);
	bool Contains(const vec3& position) const;
	bool Intersects(const Rect3D& other) const;
};


struct Box3D {		// bounding box, for collision detection
public:
	vec3 max;
	vec3 min;
	Box3D();
	Box3D(const vec3& min, const vec3& max);
	vec3 GetCenter() const;
	vec3 GetSize() const;
	float GetWidth() const;
	float GetHeight() const;
	float GetDepth() const;

	bool Contains(const vec3& vec) const;
	Rect3D GetRect() const;
};


struct alignas(16) Line3D { // data
public:
	vec3 start;
	vec3 end;
	Line3D();
	Line3D(const vec3& start, const vec3& end);
	Line3D(const vec3& origin, float pitch, float yaw, float length = 1);
	std::pair<float, float> GetPitchYawInRadian() const;
	float GetLength() const;
	vec3 GetDirection() const;
	float Length() const;
	friend ostream& operator<<(ostream& os, const Line3D& Line);
};


struct CylinderData { // data
public:
	vec3 bottom;
	vec3 top;
	float radius;
	int segment;
	CylinderData();
	CylinderData(const vec3& bottom, const vec3& top, float radius, int segment);
	vector<vec3> GetVertexBuffer();
};



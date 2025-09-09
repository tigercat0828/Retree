#pragma once
#include <glm/glm.hpp>
using glm::vec2;

struct Rect2D {
public:
	float x;
	float y;
	float width;
	float height;
	Rect2D() = default;
	Rect2D(float x, float y, float w, float h);
	Rect2D(const vec2& anchor, const vec2& size);
	bool Contains(const vec2& position) const;
	bool Intersect(const Rect2D& rect) const;
};

struct Circle {
	vec2 center;
	float radius;
	Circle() = default;
	Circle(const vec2& center, float radius);
	bool Contains(const vec2& position) const;
};

struct  alignas(16) Line2D {
	vec2 start;
	vec2 end;
	Line2D();
	Line2D(const vec2& start, const vec2& end);
	float Length() const;
};

struct Box2D { // bounding box
public:
	vec2 max;
	vec2 min;
	Box2D() = default;
	Box2D(const vec2& min, vec2& max);
	vec2 GetCenter() const;
	vec2 GetSize() const;
	bool Contains(const glm::vec2& vec) const;
	Rect2D GetRect() const;
};



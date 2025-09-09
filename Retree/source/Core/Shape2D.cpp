#include "pch.h"
#include "Core/Shape2D.h"
using namespace glm;

Rect2D::Rect2D(float x, float y, float w, float h) : x(x), y(y), width(w), height(h) {}

Rect2D::Rect2D(const vec2& anchor, const vec2& size) {
	x = anchor.x;
	y = anchor.y;
	width = size.x;
	height = size.y;
}

bool Rect2D::Contains(const vec2& position) const {
	return
		position.x >= x and position.x < x + width and
		position.y >= y and position.y < y + height;
}

bool Rect2D::Intersect(const Rect2D& other) const {
	return 
        x < other.x + other.width and
        x + width > other.x and
        y < other.y + other.height and
        y + height > other.y;
}


Box2D::Box2D(const vec2& min, vec2& max) : min(min), max(max) {}

vec2 Box2D::GetCenter() const { return (min + max) / 2.0f; }

vec2 Box2D::GetSize() const { return max - min; }

bool Box2D::Contains(const vec2& vec) const {
	return
		vec.x >= min.x and vec.x <= max.x and
		vec.y >= min.y and vec.y <= max.y;
}

Rect2D Box2D::GetRect() const {
	return Rect2D(min.x, min.y, max.x - min.x, max.y - min.y);

}

Line2D::Line2D() : start(vec2(0)), end(vec3(0)) { }

Line2D::Line2D(const vec2& start, const vec2& end) : start(start), end(end) {}

float Line2D::Length() const {
	return glm::length(start - end);
}

Circle::Circle(const vec2& center, float radius) : center(center), radius(radius) {}

bool Circle::Contains(const vec2& position) const {
	return glm::distance(center, position) <= radius;
}

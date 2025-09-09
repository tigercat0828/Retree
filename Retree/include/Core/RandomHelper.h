#pragma once
#include <glm/glm.hpp>
#include <random>
using glm::vec3;
using glm::vec2;

class RandomHelper {
private:
    static std::mt19937& GetEngine();
public:
    // [min, max] 整数
    static int RandomInt(int min, int max);

    // [min, max) 
    static float RandomFloat(float min, float max);

    // [0.0f, 1.0f) 
    static float Random();

    static vec2 RandomVec2(const vec2& min = vec2(0), const vec2& max = vec2(1));
    static vec3 RandomVec3(const vec3& min = vec3(0), const vec3& max = vec3(1));
    static vec3 RandomColor();
    static vec2 RandomInCircle(float radius=1, const vec2& center = vec2(0));
    static vec3 RandomInSphere(float radius=1, const vec3& center = vec3(0));
    static vec2 RandomOnCircle(float radius=1, const vec2& center = vec2(0));
    static vec3 RandomOnSphere(float radius=1, const vec3& center = vec3(0));
};

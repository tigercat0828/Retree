#include "pch.h"
#include "Core/RandomHelper.h"
#include <glm/gtc/random.hpp>

std::mt19937& RandomHelper::GetEngine() {
    static std::random_device rd;
    static std::mt19937 engine(rd());
    return engine;
}

int RandomHelper::RandomInt(int min, int max) {
    return std::uniform_int_distribution<int>{min, max}(GetEngine());
}

float RandomHelper::RandomFloat(float min, float max) {
    return std::uniform_real_distribution<float>{min, max}(GetEngine());
}

float RandomHelper::Random() {
    return RandomFloat(0.0f, 1.0f);
}

glm::vec2 RandomHelper::RandomVec2(const glm::vec2& min, const glm::vec2& max) {
    return {
        RandomFloat(min.x, max.x),
        RandomFloat(min.y, max.y)
    };
}

glm::vec3 RandomHelper::RandomVec3(const glm::vec3& min, const glm::vec3& max) {
    return {
        RandomFloat(min.x, max.x),
        RandomFloat(min.y, max.y),
        RandomFloat(min.z, max.z)
    };
}

glm::vec3 RandomHelper::RandomColor() {
    return glm::vec3(Random(), Random(), Random());
}

glm::vec2 RandomHelper::RandomInCircle(float radius, const glm::vec2& center){
    return center + glm::diskRand(radius);
}

glm::vec3 RandomHelper::RandomInSphere(float radius, const glm::vec3& center){
    return center + glm::ballRand(radius);
}

glm::vec2 RandomHelper::RandomOnCircle(float radius, const glm::vec2& center) {
    return center + glm::circularRand(radius);
}

glm::vec3 RandomHelper::RandomOnSphere(float radius, const glm::vec3& center) {
    return center + glm::sphericalRand(radius);
}

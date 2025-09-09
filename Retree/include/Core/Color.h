#pragma once
#include <glm/glm.hpp>
using glm::vec3;

class Color {
public:
    Color() = delete;
    ~Color() = delete;
    static constexpr vec3 Red =         vec3(1.0f, 0.0f, 0.0f);
    static constexpr vec3 Green =       vec3(0.0f, 1.0f, 0.0f);
    static constexpr vec3 LightGreen =  vec3(0.0f, 0.5f, 0.0f);
    static constexpr vec3 Blue =        vec3(0.0f, 0.0f, 1.0f);
    static constexpr vec3 White =       vec3(1.0f, 1.0f, 1.0f);
    static constexpr vec3 Black =       vec3(0.0f, 0.0f, 0.0f);
    static constexpr vec3 Yellow =      vec3(1.0f, 1.0f, 0.0f);
    static constexpr vec3 Cyan =        vec3(0.0f, 1.0f, 1.0f);
    static constexpr vec3 Magenta =     vec3(1.0f, 0.0f, 1.0f);
    static constexpr vec3 Orange =      vec3(1.0f, 0.5f, 0.0f);
    static constexpr vec3 Purple =      vec3(0.5f, 0.0f, 0.5f);
    static constexpr vec3 Gray =        vec3(0.5f, 0.5f, 0.5f);
    static constexpr vec3 LightGray =   vec3(0.8f, 0.8f, 0.8f);
    static constexpr vec3 DarkGray =    vec3(0.2f, 0.2f, 0.2f);
    static constexpr vec3 Brown =       vec3(0.5451f, 0.2706f, 0.0745f);
};
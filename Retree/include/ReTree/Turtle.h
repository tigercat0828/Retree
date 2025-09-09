#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <stack>
#include <string>
#include "Core/Shape3D.h"

using glm::vec3;
using std::vector;
using std::string;
using std::stack;
class Turtle {
    struct state {
    public:
        vec3 position;
        float pitch;
        float yaw;
        float roll;
        state(const vec3& position, float pitch, float yaw);
    };
private:
    vec3 mCurrentPosition;
    float mCurrentPitch = 90;
    float mCurrentYaw = 0;
    float mCurrentRoll = 0;
    float mStep = 0.15f;
    float mYaw = 30;
    float mPitch = 30;
    float mRoll = 30;
public:
    vector<Line3D> mPaths;
    stack<state> branches;
public:
    Turtle(const vec3& origin = vec3(0));
    Turtle(float x, float y, float z);

    void Process(const string& input);
    void Reset(const vec3& position = vec3(0));
    vector<vec3> GetVertexBuffer() const;
    vector<Line3D> GetPathLines() const;
    vector<vec3> GetStartPoints() const;
    void SetPitchYawRoll(float pitch, float yaw, float roll);
private:
    void Forward(float amount);
    void RotateYaw(float delta);
    void RotatePitch(float delta);
};
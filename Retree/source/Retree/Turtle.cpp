#include "pch.h"

#include "Retree/Turtle.h"
using namespace std;
using namespace glm;

Turtle::Turtle(const glm::vec3& origin) : mCurrentPosition(origin) {}

Turtle::Turtle(float x, float y, float z) : mCurrentPosition(x, y, z) {}

void Turtle::Process(const string& input) {
    size_t i = 0;
    while (i < input.length()) {
        char c = input[i];
        float amount = 0;
        if (i + 1 < input.length() && input[i + 1] == '(') {
            size_t start = i + 2;
            size_t end = input.find(')', start);
            if (end != string::npos) {
                string numStr = input.substr(start, end - start);
                amount = stof(numStr);
                i = end + 1;
            }
        }
        else {
            i++;
        }

        int arg;
        switch (c) {
        case 'F': {
            vec3 start = mCurrentPosition;
            Forward(amount > 0 ? amount : mStep);
            mPaths.emplace_back(start, mCurrentPosition);
            break;
        }
        case '+':
            arg = amount > 0 ? amount : mYaw;
            RotateYaw(arg);
            cout << "Yaw +" << arg << endl;
            break;
        case '-':
            arg = amount > 0 ? amount : mYaw;
            RotateYaw(amount > 0 ? -amount : -mYaw);
            cout << "Yaw -" << arg << endl;
            break;
        case '&':
            arg = amount > 0 ? amount : mPitch;
            RotatePitch(amount > 0 ? amount : mPitch);
            cout << "Pitch +" << arg << endl;
            break;
        case '^':
            arg = amount > 0 ? amount : mPitch;
            RotatePitch(amount > 0 ? -amount : -mPitch);
            cout << "Pitch -" << arg << endl;
            break;
        case '[':
            cout << "Push " << endl;
            branches.push({ mCurrentPosition, mCurrentPitch, mCurrentYaw });
            break;
        case ']':
            cout << "Pop " << endl;
            if (!branches.empty()) {
                state currState = branches.top();
                mCurrentPosition = currState.position;
                mCurrentYaw = currState.yaw;
                mCurrentPitch = currState.pitch;
                branches.pop();
            }
            break;
        }
    }
}

void Turtle::Reset(const glm::vec3& position) {
    mCurrentPosition = position;
    mPitch = 90;
    mYaw = 0;
    mStep = 1.0f;
    mPaths.clear();
    while (!branches.empty()) branches.pop();
}

vector<vec3> Turtle::GetVertexBuffer() const {
    vector<vec3> vertices;
    for (const Line3D& ln : mPaths) {
        vertices.push_back(ln.start);
        vertices.push_back(ln.end);
    }
    return vertices;
}

vector<Line3D> Turtle::GetPathLines() const {
    return mPaths;
}

vector<vec3> Turtle::GetStartPoints() const {
    vector<vec3> points;
    points.reserve(mPaths.size());
    for (const auto& line : mPaths) {
        points.push_back(line.start);
    }
    return points;
}

void Turtle::SetPitchYawRoll(float pitch, float yaw, float roll) {
    mCurrentPitch = pitch;
    mCurrentYaw = yaw;
    mCurrentRoll = roll;
}

void Turtle::RotatePitch(float delta) {
    mCurrentPitch = fmod(mCurrentPitch + delta, 360.0f);
    if (mCurrentPitch < 0) mCurrentPitch += 360.0f;
}

void Turtle::RotateYaw(float delta) {
    mCurrentYaw = fmod(mCurrentYaw + delta, 360.0f);
    if (mCurrentYaw < 0) mCurrentYaw += 360.0f;
}

void Turtle::Forward(float amount) {
    float pitchRad = glm::radians(mCurrentPitch);
    float yawRad = glm::radians(mCurrentYaw);
    vec3 dir = glm::normalize(vec3(
        cos(yawRad) * cos(pitchRad),
        sin(pitchRad),
        sin(yawRad) * cos(pitchRad)
    ));
    mCurrentPosition += dir * amount;
}

Turtle::state::state(const glm::vec3& position, float pitch, float yaw) : position(position), pitch(pitch), yaw(yaw) {}

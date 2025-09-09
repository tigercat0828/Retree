#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Core/RandomHelper.h"
#include "Core/Brush3D.h"
using glm::vec3;
using std::shared_ptr;
using std::weak_ptr;
using std::make_shared;
using std::vector;

struct ScaLeaf {
    vec3 position;
    bool isReached = false;
    ScaLeaf(const vec3& position) : position(position) {}
};

struct ScaNode : public std::enable_shared_from_this<ScaNode> {
public:
    vec3 position;
    vec3 direction;
    vec3 originalDir;
    weak_ptr<ScaNode> parent;  // 避免循環引用
    int count = 0;
    float length =0;

    ScaNode(const shared_ptr<ScaNode>& parent, const vec3& position, const vec3& direction);

    void Reset();

    shared_ptr<ScaNode> Next(float length = 0.5f);
};


class ScaTree {
public:
    float MAX_DIST = 5.0f;  // attract dist
    float MIN_DIST = 1.0f;  // leaf touching dist
    int LeafCountToStop = 10;
    float GrowLength = 0.5f;
    float MinSpacingFromPrevious = 0.3f; // 可在外部調整
    
    shared_ptr<ScaNode> root;
    vector<shared_ptr<ScaNode>> nodes;
    vector<ScaLeaf> leaves;
    
    vector<vec3> mPrevNewNodePositions;
public:
    ScaTree() = default;
    ScaTree(const vec3& rootPos, const vec3& initDirection);

    void AutoParameter(float minF = 0.08f, float maxF = 1.0f, float growF = 0.1f);

    void SetGrowParameter(float min, float max, float growLength =0.5f);

    void SpreadLeaves(const vector<vec3>& leafPositions);

    bool Grow();

    void Draw(Brush3D& brush);
    
};

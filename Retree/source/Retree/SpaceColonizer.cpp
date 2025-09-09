#include "pch.h"
#include "Core/Shape3D.h"
#include "Retree/SpaceColonizer.h"

ScaNode::ScaNode(const shared_ptr<ScaNode>& parent, const vec3& position, const vec3& direction)
    : parent(parent), position(position), direction(direction), originalDir(direction) {
}

void ScaNode::Reset() {
    count = 0;
    direction = originalDir;
}

shared_ptr<ScaNode> ScaNode::Next(float length) {
    this->length = length;
    vec3 nextPos = position + direction * length;
    Logger::Debug("node new {}", glm::to_string(nextPos));
    return make_shared<ScaNode>(shared_from_this(), nextPos, direction);
}

ScaTree::ScaTree(const vec3& rootPos, const vec3& initDirection) {
    root = make_shared<ScaNode>(nullptr, rootPos, initDirection);
    nodes.push_back(root);
}

void ScaTree::AutoParameter(float minF, float maxF, float growF) {
    if (leaves.size() < 2 || !root) return;

    float totalDist = 0.0f;
    int count = 0;

    // root 到葉子距離 (for MAX_DIST)
    float totalRootDist = 0.0f;
    for (const auto& leaf : leaves) {
        totalRootDist += glm::distance(root->position, leaf.position);
    }
    float avgRootLeafDist = totalRootDist / leaves.size();

    // ⬅️ 推導三個核心參數
    MIN_DIST = std::max(avgRootLeafDist * minF, 0.01f);
    MAX_DIST = avgRootLeafDist;
    GrowLength = std::max(avgRootLeafDist * growF * 0.5f, 0.01f);
}

/// <summary>
/// 設定生長參數
/// </summary>
/// <param name="min">the distance that touch leaf</param>
/// <param name="max">the distance will be attracted</param>
void ScaTree::SetGrowParameter(float min, float max, float growLen) {
    MIN_DIST = min;
    MAX_DIST = max;
    GrowLength = growLen;
}

void ScaTree::SpreadLeaves(const vector<vec3>& leafPositions) {
    for (const auto& pos : leafPositions) {
        leaves.emplace_back(pos);
    }
}

bool ScaTree::Grow() {
    int growedThisRound = 0;
    size_t oldLeafCount = leaves.size();

    // ---------- 吸引階段 ----------
    for (ScaLeaf& leaf : leaves) {
        shared_ptr<ScaNode> closest = nullptr;
        float minDist = MAX_DIST;

        for (const auto& node : nodes) {
            float dist = glm::distance(leaf.position, node->position);
            if (dist < MIN_DIST) {
                leaf.isReached = true;
                closest = nullptr;
                break;
            }
            else if (dist < minDist) {
                minDist = dist;
                closest = node;
            }
        }

        if (closest) {
            vec3 dir = glm::normalize(leaf.position - closest->position);
            closest->direction += dir + 0.1f * glm::normalize(RandomHelper::RandomVec3());  // add some random vector to avoid stuck
            closest->count++;
        }
    }

    // ---------- 生長新節點 ----------
    vector<shared_ptr<ScaNode>> newNodes;
    for (auto& node : nodes) {
        if (node->count > 0) {
            node->direction /= static_cast<float>(node->count);
            vec3 newPos = node->position + glm::normalize(node->direction) * GrowLength;

            // 檢查是否與上一輪新增的節點太近，若是則略過
            bool tooClose = std::any_of(mPrevNewNodePositions.begin(), mPrevNewNodePositions.end(),
                [&](const vec3& prevPos) {
                    return  glm::distance(prevPos, newPos) < MinSpacingFromPrevious;
                });

            if (!tooClose) {
                auto next = make_shared<ScaNode>(node, newPos, node->direction);
                newNodes.push_back(next);
                growedThisRound++;
            }

            node->Reset();
        }
    }
    nodes.insert(nodes.end(), newNodes.begin(), newNodes.end());

    // ---------- 更新上一輪新節點位置 ----------
    mPrevNewNodePositions.clear();
    for (const auto& n : newNodes)
        mPrevNewNodePositions.push_back(n->position);

    // ---------- 刪除達成的葉子 ----------
    leaves.erase(
        std::remove_if(leaves.begin(), leaves.end(),
            [](const ScaLeaf& l) { return l.isReached; }),
        leaves.end());

    return leaves.size() > 50;
}


void ScaTree::Draw(Brush3D& brush) {
    brush.SetColor(Color::Gray);
    for (const auto& node : nodes) {
        if (auto p = node->parent.lock()) {
            brush.DrawLine(node->position, p->position);  // 先 lock 再使用
        }
    }
    brush.SetColor(Color::Red);
    for (const auto& node : nodes) {
        brush.DrawPoint(node->position);
    }
    brush.SetPointSize(3);
    brush.SetColor(Color::Green);
    for (const auto& leaf : leaves) brush.DrawPoint(leaf.position);
}

#pragma once
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>
#include <memory>
#include <string>
#include "Retree/PosGraph.h"
#include "Core/Shape3D.h"
#include "Core/IObservable.h"
#include "Core/GameObject.h"

class ScaTree;

using std::vector;
using glm::vec3;
using std::unordered_map;
using std::unordered_set;
using std::function;
using std::pair;
using std::string;

struct TreeNode {
public:		// ctor
	// TreeNode() = default;
	TreeNode(const vec3& pos = vec3(0), float radius = 0.1f);
	~TreeNode();
public:		// property
	size_t ID = -1;
	size_t depth = 0;
	float radius = 0;	// thickness
	bool isTrunk = false;
	vec3 position = vec3(0);
	TreeNode* parent = nullptr;
	vector<TreeNode*> children;
public:
	TreeNode* CloneFromNode(TreeNode* newParent) const;
	void AddChild(TreeNode* node);
	void Print(int indent = 0);
	void Clean();
};

class Tree : public IObservable<void()>, public GameObject {

public:
	Tree() = default;
	~Tree();
	void Clean();
	Tree* CloneTree() const;	// deep copy 

private:
	TreeNode* root = nullptr;

	vector<TreeNode*> mAllNodes;
	vector<TreeNode*> mLeafNodes;
	vector<TreeNode*> mBranchNodes;
	vector<TreeNode*> mTrunkNodes;
	vector<TreeNode*> mCommonNodes;

	int mEdgeCount = 0;
	int mTrunkEdgeCount = 0;

	float mAvgDepth = 0;
	int mMaxDepth = 0;
	int mMinDepth = 100000;
	int tTotalDepth = 0;
	int tNodeCount = 0;

public: // for generate subtree
	void Truncate(TreeNode* node);
	void TrimAllLeaves();
	void TrimBranch();
	void TrimLongTwigs();
	void MergeNodes(float positionThreshold, float radiusThreshold, float degreeThreshold) {
		if (!root) return;

		float cosThreshold = std::cos(glm::radians(degreeThreshold));
		std::unordered_set<TreeNode*> toDelete;

		for (TreeNode* current : mCommonNodes) {
			// 節點被標記刪除後就略過
			if (!current || toDelete.count(current)) continue;

			if (current->children.size() != 1) continue;
			TreeNode* child = current->children[0];
			if (!child || toDelete.count(child) || child->children.size() != 1) continue;

			TreeNode* grandChild = child->children[0];
			if (!grandChild || toDelete.count(grandChild)) continue;

			// 條件判斷：位置差、半徑差、方向角度
			float dist = glm::distance(current->position, child->position);
			float rDiff = std::abs(current->radius - child->radius);
			vec3 dir1 = glm::normalize(child->position - current->position);
			vec3 dir2 = glm::normalize(grandChild->position - child->position);
			float angleCos = glm::dot(dir1, dir2);

			if (dist < positionThreshold && rDiff < radiusThreshold && angleCos > cosThreshold) {
				// 合併 current 和 child
				current->position = (current->position + child->position) * 0.5f;
				current->radius = (current->radius + child->radius) * 0.5f;

				// 移除 child 並連接 grandChild
				current->children.clear();
				current->AddChild(grandChild);
				grandChild->parent = current;

				// 標記 child 待刪除
				toDelete.insert(child);
			}
		}

		// 執行刪除
		for (TreeNode* node : toDelete) {
			if (!node) continue;
			if (node->parent) {
				auto& siblings = node->parent->children;
				siblings.erase(std::remove(siblings.begin(), siblings.end(), node), siblings.end());
			}
			delete node;
		}

		CalcTreeInfo(); // 重建節點索引與分類資訊
	}
	void CreateSmallTwigForConnection(TreeNode* tnode, const vec3& cameraPosition, bool right);

	void Perturb(float strength = 0.1f);	// todo use stohastic Lsystem grammar. now is trash

	string ToLsysGrammar();
	string ToLsysGrammar(TreeNode* node);
public:
	Tree* ExtractSubTree(TreeNode* node);
	void Connect(TreeNode* leafNode , Tree* tree);	// need to check

	void SmoothRadius(TreeNode* node, float ratio);
public: // method
	vec3 SetToLocalSpaceOrigin();
	void SetForward(const vec3& forward);

	vector<Tree*> ExtractComponentFromLeaf(int leafDepth) const;
	vector<Tree*> ExtractComponentFromTrunk(int trunkDepth) const;
	
	void CalcTreeInfo(bool doMarkTrunk = true);
	void UpdateDepth();
	void SetTransform(const Transform& transform);
	void SetPosition(const vec3& position);
	void SetRoot(TreeNode* node);
	void MoveRoot(Tree* tree);
public:		// getter
	int GetMaxDepth() const;
	int GetMinDepth() const;
	size_t GetLeafCount() const;
	size_t GetNodeCount() const;
	size_t GetBranchCount() const;
	size_t GetEdgeCount() const;
	size_t GetTrunkEdgeCount() const;
	size_t GetAvgLeafDepth() const;

	vector<vec3> GetLeafNodePosition() const;		// for skeleton node rendering
	vector<vec3> GetCommonNodePosition() const;		// for skeleton node rendering
	vector<vec3> GetBranchNodePosition() const;		// for skeleton node rendering
	vector<vec3> GetTrunkEdgeList();				// for trunk edge rendering
	vector<vec3> GetEdgeList();						// for skeleton edge rendering

	const vector<TreeNode*>& GetAllTreeNodes() const;		// for construct octree
	vector<vector<pair<vec3, float>>> GetPipeData() const;	// for building tree branch mesh
	vector<Line3D> GetLeafEdges() const;					// for adjust leaf forward

	const TreeNode* GetRoot() const;

private:	// tree info
	void calc_info();
	void mark_trunk(float radiusThreshold, float degreeThreshold);
	void clear_info();
public:	// global data
	inline static int GlobalCounter = 0;
	inline static unordered_map<size_t, TreeNode*> NodeIdSet;
public:
	static TreeNode* FindByID(int id);
	static Tree* ConstructFromGraph(const PosGraph& graph);
	static Tree* ConstructFromLstring(const string& Lstring, float degreeDiff = 0, float distDiffPercent =0);
	static Tree* ConstructFromScaTree(const ScaTree& sca, float radius);
	
public: // Debug code
	static TreeNode* MakeRandomNode(int depth = 0);
	static TreeNode* MakeRandomTree(int maxDepth = 4, int maxChildren = 4, TreeNode* parent = nullptr);
	static Tree* MakeTestTreeMid();
	static Tree* MakeTestTreeSmall();
	vector<size_t> GetRandomPath();
	void Print() const;
};

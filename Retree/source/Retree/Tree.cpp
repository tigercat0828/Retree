#include "pch.h"
#include "Retree/Tree.h"
#include "Core/RandomHelper.h"
#include "Retree/PosGraph.h"
#include "Retree/SpaceColonizer.h"

Tree::~Tree() { Clean(); }

void Tree::Clean() {
	if (root) {
		delete root;
		root = nullptr;
	}
    clear_info();
}

Tree* Tree::CloneTree() const {
    Tree* newTree = new Tree();
    if (root) {
        newTree->root = root->CloneFromNode(nullptr);
        newTree->CalcTreeInfo();
    }
    return newTree;
}

// deep copy 



TreeNode* Tree::MakeRandomNode(int depth) {
	vec3 position = RandomHelper::RandomVec3();
	float radius = RandomHelper::Random();
	TreeNode* node = new TreeNode(position, radius);
	node->depth = depth;
	return node;
}

TreeNode* Tree::MakeRandomTree(int maxDepth, int maxChildren, TreeNode* parent) {
	if (maxDepth < 0) return nullptr;

	TreeNode* root = MakeRandomNode(parent ? parent->depth + 1 : 0);
	root->parent = parent;
     
	int childCount = RandomHelper::RandomInt(1, maxChildren);
	for (int i = 0; i < childCount; ++i) {
		TreeNode* child = MakeRandomTree(maxDepth - 1, maxChildren, root);
		if (child) {
			root->AddChild(child); 
		}
	}
	return root;
}

Tree* Tree::MakeTestTreeMid() {
    
    float r = 0.1f;
    TreeNode* t0 = new TreeNode(vec3(0, 0, 0), r);

    TreeNode* t1 = new TreeNode(vec3(0, 1, 0), r);
    TreeNode* t1a = new TreeNode(vec3(1, 2, 0), r);
    TreeNode* t1b = new TreeNode(vec3(2, 3, 0), r);

    TreeNode* t2 = new TreeNode(vec3(0, 2, 0), r);
    TreeNode* t2a = new TreeNode(vec3(-1, 3, 0), r);
    TreeNode* t2b = new TreeNode(vec3(-2, 4, 0), r);

    TreeNode* t3 = new TreeNode(vec3(0, 3, 0), r);

    TreeNode* t4 = new TreeNode(vec3(0, 4, 0), r);
    TreeNode* t4a = new TreeNode(vec3(1, 5, 0), r);

    TreeNode* t5 = new TreeNode(vec3(0, 5, 0), r);
    TreeNode* t5a = new TreeNode(vec3(-1, 6, 0), r);

    TreeNode* t6 = new TreeNode(vec3(0, 6, 0), r);
    TreeNode* t7 = new TreeNode(vec3(0, 7, 0), r);

    t0->AddChild(t1);   
    t1->AddChild(t2);   t1->AddChild(t1a);  t1a->AddChild(t1b);
    t2->AddChild(t3);   t2->AddChild(t2a);  t2a->AddChild(t2b);
    t3->AddChild(t4);   
    t4->AddChild(t5);   t4->AddChild(t4a);
    t5->AddChild(t6);   t5->AddChild(t5a);
    t6->AddChild(t7);

    
    Tree* tree = new Tree();
    tree->SetRoot(t0);
    tree->CalcTreeInfo();
    return tree;
}

Tree* Tree::MakeTestTreeSmall() {
    float r = 0.2f;
    TreeNode* A = new TreeNode(vec3(0, 0, 0), r);
    TreeNode* B = new TreeNode(vec3(0, 1, 0), r);
    TreeNode* C = new TreeNode(vec3(0, 2, 0), r);
    TreeNode* D = new TreeNode(vec3(0, 3, 0), r);

    TreeNode* E = new TreeNode(vec3(-1, 2, 0), r);
    TreeNode* F = new TreeNode(vec3(1, 2, 0), r);

    TreeNode* G = new TreeNode(vec3(0, 2.5, -0.5), r);
    TreeNode* H = new TreeNode(vec3(0, 2.5, 0.5), r );

    A->AddChild(B);
    B->AddChild(C); B->AddChild(E); B->AddChild(F);
    C->AddChild(D); C->AddChild(G); C->AddChild(H);


    Tree* tree = new Tree();
    tree->SetRoot(A);
    tree->CalcTreeInfo();
    return tree;
}

vector<size_t> Tree::GetRandomPath() {
    vector<size_t> result;
    if (!root) return result;

    TreeNode* current = root;
    result.push_back(current->ID);

    std::random_device rd; std::mt19937 gen(rd());

    // DFS，隨機往下走，直到沒有 
    while (!current->children.empty()) {
        std::uniform_int_distribution<> dist(0, static_cast<size_t>(current->children.size()) - 1);
        int nextIndex = dist(gen);
        current = current->children[nextIndex];
        result.push_back(current->ID);
    }
    return result;
}

void Tree::SetTransform(const Transform& transform) {
    this->transfrom = transform;
    NotifyObservers();
}

void Tree::SetPosition(const vec3& position) {
    this->transfrom.position = position;
    NotifyObservers();
}


void Tree::Print() const {
    if(root) root->Print();
}

void Tree::SetRoot(TreeNode* node) { 
    Clean();
    root = node; 
    if (root) {
        root->isTrunk = true;
        CalcTreeInfo();
    }
}

void Tree::MoveRoot(Tree* tree) {
    if (!tree || !this->root) return;
    tree->SetRoot(this->root);
    this->root = nullptr;
}

vector<Tree*> Tree::ExtractComponentFromLeaf(int leafDepth) const {
    vector<Tree*> result;
    unordered_set<TreeNode*> visited;
    unordered_set<TreeNode*> picked;

    for (const TreeNode* node : mLeafNodes) {
        bool foundNewSubRoot = true;
        TreeNode* current = node->parent;
        for (int i = 0; i < leafDepth && current->parent; ++i) {
            if (visited.find(current) != visited.end()) {
                foundNewSubRoot = false;
                break;
            }
            visited.insert(current);
            current = current->parent;
        }
        if (foundNewSubRoot) picked.insert(current);
    }
    
    for (TreeNode* rootCandidate : picked) {
        if (!rootCandidate) continue;
        Tree* newTree = new Tree();
        TreeNode* clonedRoot = rootCandidate->CloneFromNode(nullptr);
        newTree->SetRoot(clonedRoot);
        newTree->UpdateDepth();
        if (newTree->GetMaxDepth() <= leafDepth * 2) {
            newTree->SetToLocalSpaceOrigin();
            newTree->SetForward(vec3(0, 1, 0));
            
            result.push_back(newTree);
            Logger::Debug("new compoent depth {}", newTree->GetMaxDepth());
        }
        else {
            delete newTree;
        }
    }
    return result;
}

vector<Tree*> Tree::ExtractComponentFromTrunk(int trunkDepth) const {
    // TODO : false
    // PS : leafDepth is depth count from trunk
    // if trunkDepth = 3, from the leaf nodes, 往下三層, 再開始擷取 
    vector<Tree*> result;
    vector<Tree*> temp;
    for(TreeNode* node : mTrunkNodes){
        if (!node) continue;
        // 往下 trunkDepth 層
        TreeNode* current = node;
        int currentDepth = 0;
        
        // 使用 BFS 直到 depth 達成
        std::queue<TreeNode*> q;
        q.push(current);

        while (!q.empty() && currentDepth < trunkDepth) {
            int levelSize = q.size();
            for (int i = 0; i < levelSize; ++i) {
                TreeNode* n = q.front(); q.pop();
                for (TreeNode* child : n->children) {
                    if (child->isTrunk) continue;
                    q.push(child);
                }
            }
            currentDepth++;
        }

        // 把這層的所有節點都克隆為新樹
        while (!q.empty()) {
            TreeNode* start = q.front(); q.pop();
            Tree* newTree = new Tree();
            TreeNode* clonedRoot = start->CloneFromNode(nullptr);
            newTree->SetRoot(clonedRoot);
            newTree->CalcTreeInfo();
            temp.push_back(newTree);
        }
    }

    for (Tree* tree : temp) {
        if (tree->GetMaxDepth() > 10)
            result.push_back(tree);
        else {
            delete tree;
            Logger::Info("DELETE");
        }
    }

    return result;
}


int Tree::GetMaxDepth() const { return mMaxDepth; }

int Tree::GetMinDepth() const { return mMinDepth; }

size_t Tree::GetLeafCount() const { return mLeafNodes.size(); }

size_t Tree::GetNodeCount() const { return mAllNodes.size(); }

size_t Tree::GetBranchCount() const { return mBranchNodes.size(); }

size_t Tree::GetEdgeCount() const { return mEdgeCount; }

size_t Tree::GetTrunkEdgeCount() const { return mTrunkEdgeCount; }

size_t Tree::GetAvgLeafDepth() const { return mAvgDepth; }

vector<vec3> Tree::GetLeafNodePosition() const {
    vector<vec3> result;
    result.reserve(mLeafNodes.size()); // 預先分配空間，避免多次 realloc
    for (const TreeNode* node : mLeafNodes) {
        result.push_back(node->position);
    }
    return result;
}
vector<vec3> Tree::GetCommonNodePosition() const { 
    vector<vec3> result;
    result.reserve(mLeafNodes.size()); // 預先分配空間，避免多次 realloc
    for (const TreeNode* node : mCommonNodes) {
        result.push_back(node->position);
    }
    return result;
}

vector<vec3> Tree::GetBranchNodePosition() const { 
    vector<vec3> result;
    result.reserve(mBranchNodes.size());
    for (const TreeNode* node : mBranchNodes) {
        result.push_back(node->position);
    }
    return result;
}


vector<vec3> Tree::GetTrunkEdgeList() { 
    vector<vec3> trunkEdges;
    for (TreeNode* node : mTrunkNodes) {
        for (TreeNode* child : node->children) {
            if (child->isTrunk) {
                trunkEdges.push_back(node->position);
                trunkEdges.push_back(child->position);
            }
        }
    }
    mTrunkEdgeCount = trunkEdges.size();
    return trunkEdges;
}

vector<vec3> Tree::GetEdgeList() { 
    vector<vec3> edges;
    edges.reserve(mAllNodes.size() * 1.5);
    for (TreeNode* node : mAllNodes) {
        for (TreeNode* child : node->children) {
            edges.push_back(node->position);
            edges.push_back(child->position);
        }
    }
    mEdgeCount =  edges.size();
    return edges;
}

// for construct octree
const vector<TreeNode*>& Tree::GetAllTreeNodes() const { return mAllNodes; }


void Tree::Truncate(TreeNode* node) {
    if (!node) return;
    for (TreeNode* child : node->children) {
         delete child;
    }
    node->children.clear();
    CalcTreeInfo();
}

void Tree::TrimAllLeaves() {
    if (mMaxDepth <= 2) return;
    for (TreeNode* node : mLeafNodes) {
        TreeNode* parent = node->parent;
        if (parent) {
            auto& siblings = parent->children;
            siblings.erase(std::remove(siblings.begin(), siblings.end(), node), siblings.end());
            Tree::NodeIdSet.erase(node->ID);
        }
        delete node;
        node = nullptr;
    }
    CalcTreeInfo();
}
void Tree::TrimBranch() {
    if (!root) return;
    unordered_set<TreeNode*> truncateBranch;
    for (TreeNode* leaf : mLeafNodes) {
        TreeNode* current = leaf;
        while (current && current->parent) {
            current = current->parent;
            if (current->children.size() >= 2) {
                truncateBranch.insert(current);
                break;
            }
        }
    }
    for (TreeNode* node : truncateBranch) {
        Truncate(node);
    }
    CalcTreeInfo();
}
void Tree::TrimLongTwigs() {
    std::unordered_map<TreeNode*, int> leafToBranchDepth;
    int totalDepth = 0;
    int validCount = 0;

    // 1. 計算每個 leaf 到最近的分叉點的距離
    for (TreeNode* leaf : mLeafNodes) {
        TreeNode* current = leaf;
        int depth = 0;
        while (current->parent) {
            ++depth;
            current = current->parent;
            if (current->children.size() >= 2) {
                leafToBranchDepth[leaf] = depth;
                totalDepth += depth;
                ++validCount;
                break;
            }
        }
    }

    if (validCount == 0) return;

    float avgDepth = static_cast<float>(totalDepth) / validCount;

    // 2. 統一記錄需要刪除的節點（避免重複）
    std::unordered_set<TreeNode*> truncateTargets;

    for (const auto& [leaf, depth] : leafToBranchDepth) {
        if (depth > avgDepth) {
            int overshoot = static_cast<int>(depth - avgDepth);

            TreeNode* current = leaf;
            for (int i = 0; i < overshoot && current->parent; ++i) {
                current = current->parent;
            }
            truncateTargets.insert(current);
        }
    }

    Logger::Info("Average Twig Depth : {:.2f}", avgDepth);
    Logger::Info("Deleted Twig count : {}", truncateTargets.size());

    // 3. Truncate 標記的節點
    for (TreeNode* node : truncateTargets) {
        Truncate(node);
    }

    CalcTreeInfo();
}

/// <summary>
/// create a new TreeNode that connect to the tnode 
/// the direction according the camera position
/// </summary>
void Tree::CreateSmallTwigForConnection(TreeNode* tnode, const vec3& cameraPosition, bool right) {

    if (!tnode || tnode->children.empty()) return;

    TreeNode* next = tnode->children[0];
    vec3 trunkDir = glm::normalize(next->position - tnode->position);
    float twigLength = glm::distance(tnode->position, next->position);

    vec3 nodeToCam = glm::normalize(cameraPosition - tnode->position);

    vec3 sideDir;
     if (right) 
        sideDir = glm::normalize(glm::cross(nodeToCam, trunkDir));
    else 
        sideDir = glm::normalize(glm::cross(trunkDir, nodeToCam));
    
    if (glm::length(sideDir) < 1e-5f)return;

    vec3 newTwigDir = glm::normalize(sideDir + trunkDir * 0.3f);
    vec3 newPos = tnode->position + newTwigDir * twigLength * 0.6f;
    float newRadius = tnode->radius * 0.8f;
    TreeNode* newTwig = new TreeNode(newPos, newRadius);
    
    tnode->AddChild(newTwig);
    newTwig->parent = tnode;

    CalcTreeInfo();
}


string Tree::ToLsysGrammar() {
    return ToLsysGrammar(root);
}

string Tree::ToLsysGrammar(TreeNode* node) {
    if (!node) return "";

    string result = "";
    for (TreeNode* child : node->children) {
        vec3 dir = glm::normalize(child->position - node->position);
        float distance = glm::distance(child->position, node->position);

        float yaw = std::atan2(dir.z, dir.x);
        float pitch = std::atan2(dir.y, std::sqrt(dir.x * dir.x + dir.z * dir.z));

        yaw = glm::degrees(yaw);
        pitch = glm::degrees(pitch);

        string orientation;
        if (yaw > 0) orientation        += std::format("+({})", std::abs(yaw));
        else if (yaw < 0) orientation   += std::format("-({})", std::abs(yaw));
        if (pitch > 0) orientation      += std::format("&({})", std::abs(pitch));
        else if (pitch < 0) orientation += std::format("^({})", std::abs(pitch));
        std::string move = std::format("F({})", distance);

        result +=  "["  + orientation + move + ToLsysGrammar(child) + "]";
    }
    
    return result;
}

Tree* Tree::ExtractSubTree(TreeNode* node) {
    if (!node) return nullptr;
    Tree* newTree = new Tree();
    TreeNode* newRoot = node->CloneFromNode(nullptr);
    newTree->SetRoot(newRoot);
    newTree->CalcTreeInfo();
    return newTree;
}

/// <summary>
/// should call CalcTreeInfo() after call this
/// </summary>
void Tree::Connect(TreeNode* leafNode, Tree* component) {
    if (!leafNode || !leafNode->parent || !component || !component->root) return;

    // 方向與位置對齊
    vec3 dir = glm::normalize(leafNode->position - leafNode->parent->position);
    component->SetToLocalSpaceOrigin();
    component->SetForward(dir);
    //component->SetPosition(leafNode->position);

    // 計算偏移量（leafNode 是目標對齊位置）
    vec3 offset = leafNode->position;

    // 把整棵 component 的節點位置平移（加上 offset）
    std::queue<TreeNode*> queue;
    queue.push(component->root);
    while (!queue.empty()) {
        TreeNode* current = queue.front();
        queue.pop();
        current->position += 1.001f * offset;
        for (TreeNode* child : current->children) {
            queue.push(child);
        }
    }

    leafNode->children.push_back(component->root);
    component->root->parent = leafNode;
    component->root = nullptr;

    SmoothRadius(leafNode, 0.8);

    CalcTreeInfo();
}

// need to check
void Tree::SmoothRadius(TreeNode* node, float ratio) {
    if (!node)return;

    const float baseRadius = node->radius;

    function<void(TreeNode*, int)> smooth = [&](TreeNode* current, int depth) {
        current->radius = baseRadius * std::pow(ratio, depth);
        for (TreeNode* child : current->children) {
            smooth(child, depth + 1);
        }
    };
    smooth(node, 0);
}

// need to check
void Tree::Perturb(float strength) {
    for (TreeNode* node : mAllNodes) {
        if (node->isTrunk) continue;
        node->position += RandomHelper::RandomInSphere(strength);
    }
    CalcTreeInfo();
}

// method
vec3 Tree::SetToLocalSpaceOrigin() {

    vec3 offset = root->position;
    queue<TreeNode*> queue;
    queue.push(root);
    while (!queue.empty()) {
        TreeNode* node = queue.front();
        queue.pop();
        node->position -= offset;
        for (TreeNode* child : node->children) {
            queue.push(child);
        }
    }
    CalcTreeInfo();
    return offset;
}

void Tree::SetForward(const vec3& forward) {
    if (!root || root->children.empty()) {
        Logger::Warn("root or root->children is null");
        return;
    }
    vec3 direction = glm::normalize(root->children[0]->position - root->position);
    Transform transform;
    transform.rotation = glm::rotation(direction,glm::normalize(forward));
    mat4 modelMat = transform.GetModelMatrix();
    // Step 2: apply the rotation to the line
    if (!root)return;
    queue<TreeNode*> queue;
    queue.push(root);

    while (!queue.empty()) {
        TreeNode* node = queue.front();
        queue.pop();
        node->position = vec3(modelMat * vec4(node->position, 1.0f));
        for (TreeNode* child : node->children) {
            queue.push(child);
        }
    }
    CalcTreeInfo();
}

void Tree::CalcTreeInfo(bool doMarkTrunk) {

    clear_info();

    calc_info();

    if (doMarkTrunk) mark_trunk(0.8, 20);
}
void Tree::calc_info() {

    if (!root) return;
    root->depth = 1;
    queue<TreeNode*> queue;
    queue.push(root);
    while (!queue.empty()) {
        TreeNode* current = queue.front();
        queue.pop();
        current->isTrunk = false;
        mAllNodes.push_back(current);
        tNodeCount++;

        int depth = static_cast<int>(current->depth);
        tTotalDepth += depth;


        size_t childCount = current->children.size();

        if (childCount == 0) 
            mLeafNodes.push_back(current);
        else if (childCount == 1) 
            mCommonNodes.push_back(current);
        else 
            mBranchNodes.push_back(current);

        for (TreeNode* child : current->children) {
            child->depth = current->depth + 1;
            queue.push(child);
        }
    }
    mAvgDepth = (tNodeCount > 0) ? static_cast<float>(tTotalDepth) / tNodeCount : 0.0f;

    for (TreeNode* node : mLeafNodes) {
        mMaxDepth = std::max(mMaxDepth, (int)node->depth);
        mMinDepth = std::min(mMinDepth, (int)node->depth);
    }
}


void Tree::UpdateDepth() {
    if (!root)return;
    queue<TreeNode*> queue;
    root->depth = 0;
    queue.push(root);

    int maxDepth = 0;

    while (!queue.empty()) {
        TreeNode* node = queue.front();
        queue.pop();

        if (node->depth > maxDepth) maxDepth = node->depth;

        for (TreeNode* child : node->children) {
            child->depth = node->depth + 1;
            queue.push(child);
        }
    }
    this->mMaxDepth = maxDepth;
}



void Tree::mark_trunk(float radiusThreshold, float degreeThreshold) {

    if (!root) return;

    std::stack<TreeNode*> stack;
    stack.push(root);
    root->isTrunk = true;
    mTrunkNodes.push_back(root);

    while (!stack.empty()) {
        TreeNode* node = stack.top();
        stack.pop();

        if (node->children.empty()) continue;

        if (node->children.size() == 1) {
            TreeNode* onlyChild = node->children[0];
            onlyChild->isTrunk = true;
            mTrunkNodes.push_back(onlyChild);
            stack.push(onlyChild);
            continue;
        }

        // Find the child with the largest radius
        TreeNode* baseChild = nullptr;
        float maxRadius = 0.0f;
        for (TreeNode* childNode : node->children) {
            if (childNode->radius > maxRadius) {
                maxRadius = childNode->radius;
                baseChild = childNode;
            }
        }

        vec3 baseDir = glm::normalize(baseChild->position - node->position);
        float cosAngleThreshold = std::cos(glm::radians(degreeThreshold));

        int trunkCount = 0;
        for (TreeNode* childNode : node->children) {
            float rRatio = childNode->radius / maxRadius;
            vec3 dir = glm::normalize(childNode->position - node->position);
            float cosAngle = glm::dot(baseDir, dir);

            if (rRatio >= radiusThreshold && cosAngle >= cosAngleThreshold) {
                childNode->isTrunk = true;
                trunkCount++;
                mTrunkNodes.push_back(childNode);
                stack.push(childNode);
            }
            else {
                childNode->isTrunk = false;
            }
        }

        // If no child met the condition, default to baseChild
        if (trunkCount == 0 && baseChild) {
            baseChild->isTrunk = true;
            mTrunkNodes.push_back(baseChild);
            stack.push(baseChild);
        }
    }
}
    

void Tree::clear_info() {

    mAllNodes.clear();
    mLeafNodes.clear();
    mBranchNodes.clear();
    mTrunkNodes.clear();
    mCommonNodes.clear();

   // mTrunkEdgeListPos.clear();

    mAvgDepth    = 0;
    mMaxDepth    = 0;
    mMinDepth    = 0;
    tTotalDepth  = 0;
    tNodeCount   = 0;

    tTotalDepth = 0;
    tNodeCount = 0;
    mMaxDepth = 0;
    mMinDepth = std::numeric_limits<int>::max();
}


vector<vector<pair<vec3, float>>> Tree::GetPipeData()  const {

    unordered_set<TreeNode*> visited;
    vector<vector<TreeNode*>> pipes;

    for (TreeNode* node : mLeafNodes) {
        vector<TreeNode*> path;
        visited.insert(node);
        path.push_back(node);

        while (node->parent) {
            node = node->parent;
            path.push_back(node);
            if (node == root || visited.find(node) != visited.end()) {
                break;
            }
            visited.insert(node);
        }
        std::reverse(path.begin(), path.end());

        pipes.push_back(std::move(path));
    }

    vector<vector<pair<vec3, float>>> pipeDatas;
    for (const auto& pipe : pipes) {
        vector<pair<vec3, float>> pipeData;
        for (TreeNode* node : pipe) {
            pipeData.emplace_back(node->position, node->radius);
        }
        pipeDatas.push_back(std::move(pipeData));
    }
    return pipeDatas;
}

vector<Line3D> Tree::GetLeafEdges() const {
    vector<Line3D> result;
    for(TreeNode* node : mLeafNodes) {
        if (node && node->parent) {
            result.emplace_back(node->parent->position, node->position);
        }
    }
    return result;
}

const TreeNode* Tree::GetRoot() const { return root; }

TreeNode* Tree::FindByID(int id) {
	auto it = NodeIdSet.find(id);
	return (it != NodeIdSet.end()) ? it->second : nullptr;
}

Tree* Tree::ConstructFromGraph(const PosGraph& graph) {
    Tree* tree = new Tree();
    unordered_map<TVertex, TreeNode*> vertexToNodeMap;
    std::queue<TVertex> queue;
    
    TVertex rootVertex = graph.GetRoot();
    const auto& rootProp = graph.GetVertexProp(rootVertex);
    
    TreeNode* rootNode = new TreeNode(rootProp.position, rootProp.radius);
    rootNode->depth = 0;
    rootNode->parent = nullptr;
    
    tree->root = rootNode;
    vertexToNodeMap[rootVertex] = rootNode;

    queue.push(rootVertex);

    while (!queue.empty()) {
        TVertex currentVertex = queue.front();
        queue.pop();

        TreeNode* currentNode = vertexToNodeMap[currentVertex];

        auto [begin, end] = boost::adjacent_vertices(currentVertex, graph.g);
        int childCount = 0;

        for (auto it = begin; it != end; ++it) {
            TVertex childVertex = *it;

            if (vertexToNodeMap.find(childVertex) != vertexToNodeMap.end())continue;
            const VertexProp& childProp = graph.GetVertexProp(childVertex);

            TreeNode* childNode = new TreeNode(childProp.position, childProp.radius);
            childNode->depth = currentNode->depth + 1;

            childNode->parent = currentNode;

            currentNode->AddChild(childNode);
            vertexToNodeMap[childVertex] = childNode;
            queue.push(childVertex);
        }
    }
    return tree;
}
float ParseFloatFromParen(const string str, size_t& i) {
    while (i < str.size() && str[i] != '(') ++i; // 找到 '('
    if (i >= str.size()) return 0.0f;

    size_t start = ++i; // 進到 '(' 之後
    while (i < str.size() && str[i] != ')') ++i; // 找到 ')'
    std::string numStr = str.substr(start, i - start);

    try {
        return std::stof(numStr); // 將字串轉為 float
    }
    catch (...) {
        return 0.0f;
    }
}
Tree* Tree::ConstructFromLstring(const std::string& str, float degreeDiff, float distDiffPercent) {
    struct TurtleState {
        TreeNode* node;
        vec3 position;
        TurtleState(TreeNode* n, vec3 pos) : node(n), position(pos) {}
    };
    std::default_random_engine gen(std::random_device{}());
    std::uniform_real_distribution<float> randAngle(-degreeDiff, degreeDiff);   // 絕對角度偏移
    std::uniform_real_distribution<float> randScale(1.0f - distDiffPercent, 1.0f + distDiffPercent);

    float temp_radius = 0.1f;
    std::stack<TurtleState> stack;

    TreeNode* root = new TreeNode(vec3(0), temp_radius);
    TreeNode* current = root;

    vec3 currentPos = vec3(0);
    float yaw = 0.0f;
    float pitch = 0.0f;

    size_t i = 0;
    while (i < str.size()) {
        char c = str[i];

        if (c == 'F') {
            float dist = 1.0f;
            if (i + 1 < str.size() && str[i + 1] == '(') {
                i++; // move to '('
                dist = ParseFloatFromParen(str, i);
            }
            else {
                i++;
            }
            float length = dist * randScale(gen);
            float yawRad = glm::radians(yaw + randAngle(gen));
            float pitchRad = glm::radians(pitch + randAngle(gen));
            vec3 dir = glm::normalize(vec3(
                std::cos(pitchRad) * std::cos(yawRad),
                std::sin(pitchRad),
                std::cos(pitchRad) * std::sin(yawRad)
            ));
            vec3 newPos = currentPos + dir * length;

            TreeNode* next = new TreeNode(newPos, temp_radius);
            current->AddChild(next);
            next->parent = current;
            current = next;
            currentPos = newPos;
        }
        else if (c == '+') {
            i++;    yaw = ParseFloatFromParen(str, i);
        }
        else if (c == '-') {
            i++;    yaw = -ParseFloatFromParen(str, i);
        }
        else if (c == '&') {
            i++;    pitch = ParseFloatFromParen(str, i);
        }
        else if (c == '^') {
            i++;    pitch = -ParseFloatFromParen(str, i);
        }
        else if (c == '[') {
            i++;    
            stack.push({ current, currentPos });
        }
        else if (c == ']') {
            i++;
            if (!stack.empty()) {
                auto state = stack.top(); stack.pop();
                current = state.node;
                currentPos = state.position;
                pitch = 0;
                yaw = 0;
            }
        }
        else {
            i++; // ignore unknown character
        }
    }

    Tree* tree = new Tree();
    tree->SetRoot(root);
    return tree;
}


Tree* Tree::ConstructFromScaTree(const ScaTree& sca, float baseRadius) {
    Tree* tree = new Tree();
    unordered_map<const ScaNode*, TreeNode*> nodeMap;

    // setup Node amp
    for (const auto& scaNode : sca.nodes) {
        TreeNode* node = new TreeNode(scaNode->position); // radius for temp 0.1;
        nodeMap[scaNode.get()] = node;
    }

    for (const auto& scaNode : sca.nodes) {
        TreeNode* current = nodeMap[scaNode.get()];
        if (auto p = scaNode->parent.lock()) {
            TreeNode* parent = nodeMap[p.get()];
            current->parent = parent;
            parent->AddChild(current);
        }
        else {  // no parent, it's root
            tree->SetRoot(current);
        }
    }
    tree->CalcTreeInfo();
    float falloff = 0.9f; // 可調整
    for (TreeNode* node : tree->GetAllTreeNodes()) {
        node->radius = baseRadius * std::pow(falloff, static_cast<float>(node->depth));
    }

    return tree;
}

// TreeNode ========================================================

TreeNode::TreeNode(const vec3& pos, float rad) : position(pos), radius(rad) {
    ID = Tree::GlobalCounter++;
    Tree::NodeIdSet[ID] = this;
}

TreeNode::~TreeNode() {
    for (TreeNode* child : children) {
        delete child;
    }
    children.clear();
    Tree::NodeIdSet.erase(ID);
}

TreeNode* TreeNode::CloneFromNode(TreeNode* newParent) const {

    TreeNode* copy = new TreeNode(this->position, this->radius);
    copy->depth = this->depth;
    copy->parent = newParent;

    for (TreeNode* child : this->children) {
        TreeNode* childCopy = child->CloneFromNode(copy);
        copy->AddChild(childCopy);
    }
    return copy;
}

void TreeNode::AddChild(TreeNode* node) {
    node->parent = this;
    children.push_back(node);
}

void TreeNode::Clean() {
    for (TreeNode* child : children) {
        if (child) {
            child->Clean();
            delete child;
        }
    }
    children.clear();
}

void TreeNode::Print(int indent) {
    string parentStr = parent ? std::to_string(parent->ID) : "null";
    Logger::Info("{} Node({} Depth:{}, Position:{}, Radius:{}) Parent:{}", std::string(indent, ' '), ID, depth, glm::to_string(position), radius, parentStr);
    //Logger::Info("{} {}", std::string(indent, ' '), ID);
    //Logger::Info("{} {}({})", std::string(indent, ' '), ID, parentStr);
    for (TreeNode* child : children) {
        child->Print(indent + 4);
    }
}

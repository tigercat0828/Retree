#include "pch.h"
#include "Retree/PosGraph.h"
#include "Core/Camera.h"
#include "Core/ShaderManager.h"
#include "Retree/TreeSkeletonRenderer.h"


using namespace glm;
using namespace std;
namespace fs = std::filesystem;

PosGraph::PosGraph() {
	mPositionMap.clear();
}

PosGraph::PosGraph(const vec3& root) {
	mPositionMap.clear();
	auto v = AddVertex(root);
	SetRoot(v);
}

PosGraph::PosGraph(const string& filepath) {
	mPositionMap.clear();
	fs::path path = filepath;
	string extension = path.extension().string();
	string filename = path.filename().string();
	mName = filename;
	if (extension == ".xyz")
		LoadFromXYZ(filepath);
	else if (extension == ".txt") 
		LoadFromTXT(filepath);
	else {
		Logger::Error("Unsupported file format for Posgraph");
		cin.get();
	}
}



// getter-setter
std::vector<BranchData> PosGraph::GetBranchDatas() const { return mBranchDatas; }

// getter-setter
string PosGraph::GetName() const { return mName; }

void PosGraph::SetName(const string& name) {
	mName = name;
}

// getter-setter
size_t PosGraph::GetVertexCount() const { return boost::num_vertices(g); }

size_t PosGraph::GetEdgeCount() const { return boost::num_edges(g); }


VertexProp PosGraph::GetVertexProp(TVertex vertex) const { return g[vertex]; }

EdgeProp PosGraph::GetEdgeProp(TVertex v1, TVertex v2) const {
	auto [edge, exists] = boost::edge(v1, v2, g);
	if (!exists) throw std::runtime_error("Edge not exist");
	return g[edge];
}



std::vector<glm::vec3> PosGraph::GetVerticesList() const {
	vector<vec3> vertex_positions;
	vertex_positions.reserve(GetVertexCount());
	auto [v_beg, v_end] = boost::vertices(g);
	for (const TVertex& v : boost::make_iterator_range(v_beg, v_end)) {
		vertex_positions.emplace_back(g[v].position);
	}
	return vertex_positions;
}

std::vector<glm::vec3> PosGraph::GetEdgesList() const {
	vector<vec3> edge_positions;
	edge_positions.reserve(GetEdgeCount() * 2);
	auto [e_beg, e_end] = boost::edges(g);
	for (const TEdge& e : boost::make_iterator_range(e_beg, e_end)) {
		TVertex v1 = boost::source(e, g);
		TVertex v2 = boost::target(e, g);
		edge_positions.emplace_back(g[v1].position);
		edge_positions.emplace_back(g[v2].position);
	}
	return edge_positions;
}

void PosGraph::SetRoot(TVertex vertex) { 
	if (IsExistVertex(vertex)) 	mRoot = vertex;
	else throw std::runtime_error("Vertex " + std::to_string(vertex) + "is not exist");
}

TVertex PosGraph::GetRoot() const { return mRoot; }

TVertex PosGraph::GetVertexByPosition(const glm::vec3& position) {
    auto it = mPositionMap.find(position);
	if (it != mPositionMap.end()) {
		return it->second;
	}
	return graph_t::null_vertex();
}

bool PosGraph::IsRootNull() const {
	return mRoot == graph_t::null_vertex();
}

// method
bool PosGraph::IsExistVertex(TVertex vertex) const {
	return boost::vertex(vertex, g) != graph_t::null_vertex();
}

bool PosGraph::IsExistEdge(TVertex v1, TVertex v2)const {
	return boost::edge(v1, v2, g).second;
}

TEdge PosGraph::AddEdge(TVertex v1, TVertex v2, float pitch, float yaw) {
	return boost::add_edge(v1, v2, EdgeProp{ pitch, yaw }, g).first;
}

TEdge PosGraph::AddEdge(const vec3& start, const vec3& end) {
	TVertex v1 = AddVertex(start);
	TVertex v2 = AddVertex(end);
	float pitch, yaw;
	calc_pitch_yaw(g[v1].position, g[v2].position, pitch, yaw); // noted
    return boost::add_edge(v1, v2, EdgeProp{ pitch, yaw }, g).first;
}

TEdge PosGraph::AddEdge(const glm::vec3& p1, const glm::vec3& p2, float r1, float r2) {
	TVertex v1 = AddVertex(p1, r1);
	TVertex v2 = AddVertex(p2, r2);
	float pitch, yaw;
	calc_pitch_yaw(g[v1].position, g[v2].position, pitch, yaw); // noted
	return boost::add_edge(v1, v2, EdgeProp{ pitch, yaw }, g).first;
}

TVertex PosGraph::AddVertex(const vec3& position) {
	auto it = mPositionMap.find(position);
	if (it != mPositionMap.end()) {
		// Logger::Debug("Graph \"{}\" a lready have vertex at {}", mName, glm::to_string(position));
		return it->second;
	}

	TVertex v = boost::add_vertex(VertexProp{ position }, g);
	mPositionMap[position] = v;
	return v;
}

TVertex PosGraph::AddVertex(const glm::vec3& position, float radius) {
	auto it = mPositionMap.find(position);
	if (it != mPositionMap.end()) {
		// Logger::Debug("Graph \"{}\" a lready have vertex at {}", mName, glm::to_string(position));
		return it->second;
	}

	TVertex v = boost::add_vertex(VertexProp{ position, radius }, g);
	mPositionMap[position] = v;
	return v;
}

bool PosGraph::RemoveVertex(TVertex vertex) {

	if (IsExistVertex(vertex)) {

		const vec3 pos = g[vertex].position;
		mPositionMap.erase(pos);

		if (mRoot == vertex) SetRoot(graph_t::null_vertex());
		boost::remove_vertex(vertex, g);
		return true;
	}
	else {
		printf("No such vertex %zu\n", vertex);
		// throw std::runtime_error("Vertex " + std::to_string(vertex) + "is not exist");
		return false;
	}
}

bool PosGraph::RemoveEdge(TVertex v1, TVertex v2) {
	auto [edge, exists] = boost::edge(v1, v2, g);

	if (exists) {
		boost::remove_edge(edge, g);
		return true;
	}
	else {
		printf("No such edge %zu-%zu\n", v1, v2);
		return false;
	}
}

bool PosGraph::EditVertex(TVertex vertex,const vec3& position) {
	if (!IsExistVertex(vertex)) return false;
	const vec3 oldPos = g[vertex].position;
	g[vertex].position = position;
	mPositionMap.erase(oldPos);
    mPositionMap[position] = vertex;
	return true;
}

bool PosGraph::EditEdge(TEdge edge, float pitch, float yaw) {
	g[edge].pitch = pitch;
	g[edge].yaw = yaw;
	return true;
}

bool PosGraph::EditEdge(TVertex v1, TVertex v2, float pitch, float yaw) {
	auto [edge, exists] = boost::edge(v1, v2, g);
	if (exists) {
		g[edge].pitch = pitch;
		g[edge].yaw = yaw;
		return true;
	}
	else {
        printf("No such edge %zu-%zu\n", v1, v2);
		return false;
	}
}

void PosGraph::CalculateEdgeProp() {
	auto [e_beg, e_end] = boost::edges(g);
	for (const TEdge& e : boost::make_iterator_range(e_beg, e_end)) {
		TVertex v1 = boost::source(e, g);
		TVertex v2 = boost::target(e, g);
		calc_pitch_yaw(g[v1].position, g[v2].position, g[e].pitch, g[e].yaw);
	}
}

std::vector<TVertex> PosGraph::GetLeafNodes() const {
	std::vector<TVertex> leaves;
	leaves.reserve(boost::num_vertices(g) / 4);
	auto [v_beg, v_end] = boost::vertices(g);
	for (auto it = v_beg; it != v_end; ++it) {
		const TVertex v = *it;
		// 檢查出度為0且入度至少1（避免孤立節點被誤判為葉子）
		if (boost::out_degree(v, g) == 0 && boost::in_degree(v, g) > 0) {
			leaves.push_back(v);
		}
	}
	return leaves;
}


std::vector<glm::vec3> PosGraph::GetLeafPositions() const {
	
	const auto leafNodes = GetLeafNodes();  // 複用已有方法
	std::vector<glm::vec3> positions;
	positions.reserve(leafNodes.size());    // 預分配記憶體

	for (const TVertex v : leafNodes) {
		if (IsExistVertex(v)) {  // 確保頂點未被刪除
			positions.emplace_back(g[v].position);
		}
	}
	return positions;
}

int PosGraph::GetDepth() const {
	if (IsRootNull()) return 0; // 若根节点未设置，深度为0

	std::queue<std::pair<TVertex, int>> nodeQueue; // 存储（顶点，当前深度）
	std::unordered_map<TVertex, bool> visited;
	int maxDepth = 0;

	// 初始化：根节点深度为0
	nodeQueue.emplace(mRoot, 0);
	visited[mRoot] = true;

	while (!nodeQueue.empty()) {
		auto [current, depth] = nodeQueue.front();
		nodeQueue.pop();

		maxDepth = std::max(maxDepth, depth); // 更新最大深度

		// 遍历所有邻接顶点（子节点）
		auto [beg, end] = boost::adjacent_vertices(current, g);
		for (auto it = beg; it != end; ++it) {
			TVertex neighbor = *it;
			if (!visited[neighbor]) {
				visited[neighbor] = true;
				nodeQueue.emplace(neighbor, depth + 1);
			}
		}
	}
	return maxDepth;
}

void PosGraph::RemoveLeafNodes() {
	auto leaves = GetLeafNodes();  
	for (TVertex v : leaves) {
		Logger::Debug("Remove {}", v);
		RemoveVertex(v);           
	}
}

void PosGraph::calc_pitch_yaw(const vec3& start, const vec3& end, float& pitch, float& yaw) {
	vec3 dir = glm::normalize(end - start);
	//yaw = std::atan2(dir.z, dir.x);
	//pitch = std::atan2(dir.y, sqrt(dir.x * dir.x + dir.z * dir.z));
	yaw = glm::degrees(std::atan2(dir.z, dir.x));
	pitch = glm::degrees(std::atan2(dir.y, sqrt(dir.x * dir.x + dir.z * dir.z)));
}

void PosGraph::Print() const {
	cout << "Name: " << mName << '\n';
	
	IsRootNull() ? 
		cout << "Root: null" << '\n' :
		cout << "Root: " << mRoot << '\n';

	cout << "========================================\n";
	PrintVertices();
	cout << "----------------------------------------\n";
	//PrintEdges();
	cout << "========================================\n" << endl;
}

void PosGraph::PrintVertices() const {
	auto [v_beg, v_end] = boost::vertices(g);
	for (const TVertex& v : boost::make_iterator_range(v_beg, v_end)) {
		std::print("{} : {} {}\n", v, glm::to_string(g[v].position),g[v].radius);
	}
}

void PosGraph::PrintEdges() const {
	auto [e_beg, e_end] = boost::edges(g);
	for (const TEdge& e : boost::make_iterator_range(e_beg, e_end)) {
		TVertex v1 = boost::source(e, g);
		TVertex v2 = boost::target(e, g);
		
		float pitch = g[e].pitch;
		float yaw = g[e].yaw;
		std::print("{}-{} [{}, {}]\n", v1, v2, pitch, yaw);
	}
}

// load/save file TODO;
void PosGraph::Load() {}

void PosGraph::Save() {}

vector<size_t> PosGraph::GetVertexIndices() const {
	std::vector<size_t> vertices;
	auto [v_beg, v_end] = boost::vertices(g);
	for (const TVertex& v : boost::make_iterator_range(v_beg, v_end)) {
		vertices.push_back(v);
	}
	return vertices;
}

vector<pair<size_t, size_t>> PosGraph::GetEdgeIndices() const {
	std::vector<std::pair<size_t, size_t>> edges;
	auto [e_beg, e_end] = boost::edges(g);
	for (const TEdge& e : boost::make_iterator_range(e_beg, e_end)) {
		TVertex v1 = boost::source(e, g);
		TVertex v2 = boost::target(e, g);

		edges.emplace_back(v1,v2);
	}
	return edges;
}

PosGraph PosGraph::Copy(string name) const { // deepcopy
	PosGraph newG;
	newG.mName = name;
	std::unordered_map<TVertex, TVertex> vertexMap;

	// copy vertices
	auto [v_beg, v_end] = boost::vertices(g);
	for (const TVertex& oldV : boost::make_iterator_range(v_beg, v_end)) {	// old vertex
		TVertex newV = boost::add_vertex(g[oldV], newG.g);	// new vertex
		vertexMap[oldV] = newV;
	}
	// copy edges
	auto [e_beg, e_end] = boost::edges(g);
	for (const TEdge& oldE : boost::make_iterator_range(e_beg, e_end)) {	// old edge
		TVertex source = boost::source(oldE, g);
		TVertex target = boost::target(oldE, g);
		auto newE = boost::add_edge(vertexMap[source], vertexMap[target], g[oldE], newG.g);
	}

	newG.mRoot = 
		(mRoot == graph_t::null_vertex()) ? graph_t::null_vertex() : vertexMap[mRoot]; 

	newG.mPositionMap = this->mPositionMap;
	return newG;
}

void PosGraph::LoadFromXYZ(const std::string& filename) {	// TODO : fix format x,y,z,r

	std::ifstream inputFile(filename);
	if (!inputFile.is_open()) {
		Logger::Error("Failed to open file \"{}\"", filename);
		throw std::runtime_error("Failed to open input file");
	}
	
	float x1, y1, z1;
	float x2, y2, z2;
	vec3 root(0, std::numeric_limits<float>::max(),0);


	while (inputFile >> x1 >> y1 >> z1 >> x2 >> y2 >> z2) {
		vec3 v1(x1, y1, z1);
        vec3 v2(x2, y2, z2);

		AddEdge(v1, v2);
		if (y1 < root.y) root = v1;
        if (y2 < root.y) root = v2;
	}
	size_t rootV = GetVertexByPosition(root);
	SetRoot(rootV);
	inputFile.close();
}

void PosGraph::LoadFromTXT(const std::string& filename) {
	std::ifstream inputFile(filename);
	if (!inputFile.is_open()) {
		Logger::Error("Failed to open file \"{}\"", filename);
		throw std::runtime_error("Failed to open input file");
	}

	float x1, y1, z1;
	float x2, y2, z2;
	float r1, r2;
	vec3 root(0, std::numeric_limits<float>::max(), 0);



	while (inputFile >> x1 >> y1 >> z1 >> r1 >> x2 >> y2 >> z2 >> r2) {
		vec3 v1(x1, y1, z1);
		vec3 v2(x2, y2, z2);
	

		AddEdge(v1, v2, r1, r2);
		if (y1 < root.y) root = v1;
		if (y2 < root.y) root = v2;

		mBranchDatas.emplace_back(v1, v2, r1, r2);
	}

	size_t rootV = GetVertexByPosition(root);
	SetRoot(rootV);
	inputFile.close();
}

vector<TVertex> PosGraph::TraverseBreadthFirst(TVertex start) const {

	if (!IsExistVertex(start)) return std::vector<TVertex>();

	std::vector<TVertex> orders;
	std::queue<TVertex> queue;
	std::unordered_set<TVertex> visited;

	queue.push(start);
	visited.insert(start);

	while (!queue.empty()) {
		TVertex current = queue.front();
		queue.pop();
		orders.push_back(current);

		auto [beg, end] = boost::adjacent_vertices(current, g);
		for (auto it = beg; it != end; ++it) {
			if (visited.insert(*it).second) {
				queue.push(*it);
			}
		}
	}
	return orders;
}

vector<TVertex> PosGraph::TraverseDepthFirst(TVertex start) const {

	if (!IsExistVertex(start)) return std::vector<TVertex>();

	std::vector<TVertex> orders;
	std::stack<TVertex> stack;
	std::unordered_set<TVertex> visited;

	stack.push(start);

	while (!stack.empty()) {
		TVertex	current = stack.top();
		stack.pop();
		if (visited.insert(current).second) {
			orders.push_back(current);

			auto [beg, end] = boost::adjacent_vertices(current, g);
			std::vector<TVertex> neighbors(beg, end);
			std::reverse(neighbors.begin(), neighbors.end());
			for (const auto& v : neighbors) {
				stack.push(v);
			}
		}
	}
	return orders;
}

vector<vector<TVertex>> PosGraph::TraverseLevelOrder(TVertex start) const {
	if (!IsExistVertex(start)) return std::vector<std::vector<TVertex>>();

	std::vector<std::vector<TVertex>> levels;
	std::queue<TVertex> queue;
	std::unordered_set<TVertex> visited;

	queue.push(start);
	visited.insert(start);

	while (!queue.empty()) {
		size_t level_size = queue.size();
		std::vector<TVertex> current_level;

		for (size_t i = 0; i < level_size; ++i) {
			TVertex current = queue.front();
			queue.pop();
			current_level.push_back(current);

			auto [beg, end] = boost::adjacent_vertices(current, g);
			std::vector<TVertex> neighbors(beg, end);
			std::reverse(neighbors.begin(), neighbors.end());
			for (const auto& v : neighbors) {
				if (visited.insert(v).second)
					queue.push(v);
			}
		}
		levels.push_back(std::move(current_level));
	}

	return levels;
}

// ==========================================================================

VertexProp::VertexProp(const vec3& pos) : position(pos) {}

VertexProp::VertexProp(const glm::vec3& pos, float r) : position(pos), radius(r) {}

EdgeProp::EdgeProp(float p, float y) : pitch(p), yaw(y) {}

BranchData::BranchData(const vec3& pos1, const vec3& pos2, float r1, float r2) : position1(pos1), position2(pos2), radius1(r1), radius2(r2) {}

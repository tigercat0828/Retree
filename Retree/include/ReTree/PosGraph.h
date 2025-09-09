#pragma once
#include <boost/graph/adjacency_list.hpp>
#include <boost/functional/hash.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <string>
#include <unordered_set>

class Camera;
using glm::vec3;
using std::vector;
using std::string;
using std::pair;

struct alignas(16) BranchData {
public:
	vec3 position1;
	vec3 position2;
	float radius1;
	float radius2;
	BranchData() = default;
	BranchData(const vec3& pos1, const vec3& pos2, float r1, float r2);
};

struct VertexProp {
	vec3 position = vec3(0);
	float radius = 0.0f;
	VertexProp() = default;
	VertexProp(const vec3& pos);
	VertexProp(const vec3& pos, float r);
};
struct EdgeProp {
	float pitch =0;	// radian
	float yaw =0;		// radian
	EdgeProp() = default;
    EdgeProp(float p, float y);
};

using graph_t = boost::adjacency_list<
	boost::vecS,			// adjEdge container
	boost::vecS,			// vertex container
	boost::bidirectionalS,	// directed
	VertexProp,				// vertex prop
	EdgeProp,				// edge prop
	boost::no_property		// graph prop
>;
// using Tvertex = graph_traits<Graph>::vertex_descriptor;	// traits for more general usage
using TVertex = graph_t::vertex_descriptor;	// tree vertex
using TEdge = graph_t::edge_descriptor;
using GLuint = unsigned int;

struct Vec3KeyHasher {
	size_t operator()(const vec3& v) const {
		// return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1) ^ (std::hash<float>()(v.z) << 2);
		static constexpr float EPSILON = 1e-5f;
		auto quantize = [](float val) {return std::round(val / EPSILON) * EPSILON;};
        return std::hash<float>()(quantize(v.x)) ^ 
              (std::hash<float>()(quantize(v.y)) << 1) ^ 
              (std::hash<float>()(quantize(v.z)) << 2);
		
	}
};

class PosGraph  {
public:		// ctor
	PosGraph();
	PosGraph(const vec3& root);
	PosGraph(const string& filename);
	graph_t g;
private:	// property
	string mName;
	TVertex mRoot = graph_t::null_vertex();
	unordered_map<vec3, TVertex, Vec3KeyHasher> mPositionMap;
	vector<BranchData> mBranchDatas;

public:		// getter-setter
	vector<BranchData> GetBranchDatas() const;
	string GetName() const;
	void SetName(const string& name);

	size_t GetVertexCount() const;
	size_t GetEdgeCount() const;

	VertexProp GetVertexProp(TVertex vertex) const;
	EdgeProp GetEdgeProp(TVertex v1, TVertex v2) const;

	vector<vec3> GetVerticesList() const;
	vector<vec3> GetEdgesList() const;

	vector<size_t> GetVertexIndices() const;
	vector<std::pair<size_t, size_t>> GetEdgeIndices() const;

	void SetRoot(TVertex vertex);
	TVertex GetRoot() const;
	TVertex GetVertexByPosition(const vec3& position);
public:		// method

	
	bool IsRootNull() const;
	bool IsExistVertex(TVertex vertex)const;
	bool IsExistEdge(TVertex v1, TVertex v2)const;

	TVertex AddVertex(const vec3& position);
	TVertex AddVertex(const vec3& position, float radius);
	TEdge AddEdge(TVertex v1, TVertex v2, float pitch, float yaw);
	TEdge AddEdge(const vec3& start,const vec3 &end);
	TEdge AddEdge(const vec3& p1, const vec3& p2, float r1, float r2);

	bool RemoveVertex(TVertex vertex);
	bool RemoveEdge(TVertex v1, TVertex v2);
	bool EditVertex(TVertex vertex,const vec3& position);
	bool EditEdge(TEdge edge, float pitch, float yaw);
	bool EditEdge(TVertex v1, TVertex v2, float pitch, float yaw);
	void CalculateEdgeProp();

	vector<TVertex> GetLeafNodes() const;
	vector<vec3> GetLeafPositions() const;
	int GetDepth() const;

	void RemoveLeafNodes(); // TODO wait fix

	PosGraph Copy(std::string name = "") const;

public:	// Traversing
	vector<TVertex> TraverseBreadthFirst(TVertex start) const;
	vector<TVertex> TraverseDepthFirst(TVertex start) const;
	vector<vector<TVertex>> TraverseLevelOrder(TVertex start) const;


private:
	void calc_pitch_yaw(const vec3& start, const vec3& end, float& pitch, float& yaw);
public:		// for debug
	void Print() const;
	void PrintVertices() const;
	void PrintEdges() const;

public: // load/save file TODO;
	void Load();
	void Save();
private:
	void LoadFromXYZ(const string& filename);
	void LoadFromTXT(const string& filename);

};


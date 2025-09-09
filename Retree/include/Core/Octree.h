#pragma once
#include <memory>
#include <utility>
#include <vector>
#include <cmath>
#include <array>
#include <glm/glm.hpp>
#include "Core/Shape3D.h"
#include "Core/IObservable.h"

using std::vector;
using std::queue;
using std::pair;
using std::unique_ptr;
using glm::vec3;
using std::array;

template<typename T> class Octree; // Forward declare first

template<typename T>
class OctreeNode {
	friend class Octree<T>; 
private:
	vector<pair<vec3, T> >items;
	int CAPACITY{ 8 };
	Rect3D boundary;
	unique_ptr<OctreeNode<T>> nodes[8];
	bool isDivided{ false };
public:
	OctreeNode(const Rect3D& boundary, int capacity) :boundary(boundary), CAPACITY(capacity) {}
	bool Insert(const vec3& position, const T& object) {
		if (!boundary.Contains(position)) return false;

		if (!isDivided && items.size() < CAPACITY) {
			items.emplace_back(position, object);
			return true;
		}

		if (!isDivided) {
			Subdivide();
			isDivided = true;
			for (const auto& pair : items) {
				for (int i = 0; i < 8; i++) {
					if (nodes[i]->Insert(pair.first, pair.second))break;;
				}
			}
			items.clear();
		}

		for (int i = 0; i < 8; i++) {
			if (nodes[i]->Insert(position, object)) return true;
		}
		return false;
	}
	void Query(const Rect3D& range, std::vector<T>& result) const {
		if (!boundary.Intersects(range)) return;

		for (const auto& pair : items) {
			if (range.Contains(pair.first)) {
				result.push_back(pair.second);
			}
		}
		if (isDivided) {
			for (int i = 0; i < 8; i++) {
				nodes[i]->Query(range, result);
			}
		}

	}
	int Depth()const {
		if (!isDivided) return 1;
		int max = 0;
		for (int i = 0; i < 8; i++) {
			max = std::max(max, nodes[i]->Depth());
		}
		return  1 + max;
	}
	void Clear() {
		items.clear();
		if (isDivided) {
			for (int i = 0; i < 8; i++) {
				if (nodes[i]) {
					nodes[i]->Clear();
					nodes[i].reset();
				}
			}
		}
		isDivided = false;
	}
	void Reset(const Rect3D& boundary) {
		Clear();
		this->boundary = boundary;
	}
	array<vec3, 8> GetCorners() const {

		float x = boundary.x;
		float y = boundary.y;
		float z = boundary.z;
		float w = boundary.width;
		float h = boundary.height;
		float d = boundary.depth;

		return {
			vec3(x,     y,		z),			// 0: left-bottom-back
			vec3(x + w, y,		z),			// 1: right-bottom-back
			vec3(x + w, y + h,  z),			// 2: right-top-back
			vec3(x,     y + h,  z),			// 3: left-top-back
			vec3(x,     y,		z + d),		// 4: left-bottom-front
			vec3(x + w, y,		z + d),		// 5: right-bottom-front
			vec3(x + w, y + h,  z + d),		// 6: right-top-front
			vec3(x,     y + h,  z + d)		// 7: left-top-front
		};
	}
private:
	void Subdivide() {
		float x = boundary.x;
		float y = boundary.y;
		float z = boundary.z;

		float w = boundary.width / 2.0f;
		float h = boundary.height / 2.0f;
		float d = boundary.depth / 2.0f;
		nodes[0] = std::make_unique<OctreeNode<T>>(Rect3D(x, y, z, w, h, d), CAPACITY);
		nodes[1] = std::make_unique<OctreeNode<T>>(Rect3D(x + w, y, z, w, h, d), CAPACITY);
		nodes[2] = std::make_unique<OctreeNode<T>>(Rect3D(x, y + h, z, w, h, d), CAPACITY);
		nodes[3] = std::make_unique<OctreeNode<T>>(Rect3D(x + w, y + h, z, w, h, d), CAPACITY);

		nodes[4] = std::make_unique<OctreeNode<T>>(Rect3D(x, y, z + d, w, h, d), CAPACITY);
		nodes[5] = std::make_unique<OctreeNode<T>>(Rect3D(x + w, y, z + d, w, h, d), CAPACITY);
		nodes[6] = std::make_unique<OctreeNode<T>>(Rect3D(x, y + h, z + d, w, h, d), CAPACITY);
		nodes[7] = std::make_unique<OctreeNode<T>>(Rect3D(x + w, y + h, z + d, w, h, d), CAPACITY);
	}
};

template<typename T>
class Octree : public IObservable<void()> {

private:
	unique_ptr<OctreeNode<T>> root;
	Rect3D rootBoundary;
	int capacity = 8;

public:
    Octree() = default;
	Octree(const Rect3D& boundary, int capacity = 8) : rootBoundary(boundary), capacity(capacity) {
		root = std::make_unique<OctreeNode<T>>(rootBoundary, capacity);
	}

	void Insert(const vec3& pos, const T& object) {
		if (!root->Insert(pos, object)) {
			Logger::Warn("Octree insert failed: position out of bounds {}", glm::to_string(pos));
		}
	}

	vector<T> Query(const Rect3D& range) const {
		vector<T> result;
		root->Query(range, result);
		return result;
	}

	void Clear() {
		root->Clear();
	}

	void Reset(const Rect3D& boundary) {
		rootBoundary = boundary;
		root->Reset(boundary);
	}

	int Depth() const {
		return root->Depth();
	}
	 
	pair<vector<vec3>, vector<unsigned int>> GetGLBuffer() {
		// lines
		vector<vec3> linesBuffer;	// octree boundary, 8 vertex
		vector<unsigned int> linesIndex;	// index of linesBuffer

		const array<pair<int, int>, 12> edges = { {
			{0,1}, {1,2}, {2,3}, {3,0}, // back face edges
			{4,5}, {5,6}, {6,7}, {7,4}, // front face edges
			{0,4}, {1,5}, {2,6}, {3,7}  // side edges 
		} };
		
		// BFS
        std::queue<OctreeNode<T>*> queue;
		queue.push(root.get());
		unsigned int currentVertexOffset = 0;

		while (!queue.empty()) {
			OctreeNode<T>* node = queue.front();
			queue.pop();
			if (!node) continue;

			auto corners = node->GetCorners();
			for (const auto& c : corners) {
				linesBuffer.push_back(c);
			}

			for (const auto& e : edges) {
				linesIndex.push_back(currentVertexOffset + e.first);
				linesIndex.push_back(currentVertexOffset + e.second);
			}
            currentVertexOffset += 8;
			
			if (node->isDivided) {
				for (int i = 0; i < 8; ++i) {
					if (node->nodes[i]) 
						queue.push(node->nodes[i].get());
				}
			}
		} 
		return std::make_pair(linesBuffer, linesIndex);
	}
};

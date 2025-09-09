#pragma once
#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "Core/Shape3D.h"
#include "Core/Shader.h"
#include "Core/GameObject.h"
#include "Core/IObservable.h"

using std::ifstream;
using std::vector;
using glm::vec3;
using std::function;
using std::shared_ptr;
using std::ofstream;
using std::ifstream;

class Camera;

enum AXIS {
	X_AXIS = 0b100,
	Y_AXIS = 0b010,
	Z_AXIS = 0b001,
};
class PointCloud : public GameObject, public IObservable<void()> {
public:		// constructor
	PointCloud() = default;
	PointCloud(const string& filepath);
	PointCloud(const vector<vec3>& points, const vec3& color = vec3(0.5f));
	~PointCloud();
protected:	// properties
	vector<vec3> mPositions;
	vector<vec3> mColors;
	Box3D mBoundingBox;

public:		// getter-setter
	vector<vec3> GetPoints(const Box3D& box) const;
	const vector<vec3>& GetPositions() const;
	const vector<vec3>& GetColors() const;
	Box3D GetBoundingBox() const;
	size_t GetCount() const;
public:		// methods
	void SetPosition(const vec3& position);
	void SetPointColorGradient(const vec3& top, const vec3& bottom);
	void SetPointColor(const vec3& color);
	void Flip(int axisMask);
	void Swap(int axisMask);
	void Translate(const vec3& offset);
	void AlignToOrigin();
	string SavePLY(const string& filename) const;
	string SaveXYZ(const string& filename) const;
	string SaveXZY(const string& filename) const; 
protected:
	void CheckPointCountMatched();
	void CalcProperties();

	void LoadFromXYZ(const string& filename);
	void LoadFromPLY(const string& filename);
	void ReadFile_doublePosition_ucharColor(ifstream& file, size_t count);
	void ReadFile_floatPosition_floatNormal_ucharColor(ifstream& file, size_t count);
};


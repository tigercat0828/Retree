#include "pch.h"
#include "Core/ShaderManager.h"
#include "Core/Camera.h"

#include "Retree/PointCloud.h"


PointCloud::PointCloud(const string& filepath) {
	fs::path path =  filepath; // use fullpath, may disable use file dialog...
	string extension = path.extension().string();
	string filename = path.filename().string();

	if (extension == ".xyz") {
		LoadFromXYZ(path.string().c_str());
		SetPointColor(Color::White);
	}
	else if (extension == ".ply") {
		LoadFromPLY(path.string().c_str());
	}
	else {
		Logger::Error("Unsupported file format: {}\n", filename);
		throw std::runtime_error("Unsupported file format");
	}
	CheckPointCountMatched();
	Logger::Info("{} points loaded.", GetCount());
	CalcProperties();
}

PointCloud::PointCloud(const vector<vec3>& points, const vec3& color) {
	mPositions = points;
	SetPointColor(color);
	CheckPointCountMatched();
	Logger::Info("{} points loaded.", GetCount());
	CalcProperties();
}

PointCloud::~PointCloud() {
	
}


vector<vec3> PointCloud::GetPoints(const Box3D& box) const {
	// Add the point from mPosition if it is inside the box.
	vector<vec3> points;
	points.reserve(mPositions.size() / 2);
	for (const auto& pos : mPositions) {
		if (box.Contains(pos)) {
			points.emplace_back(pos);
		}
	}
	return points;
}


const std::vector<glm::vec3>& PointCloud::GetPositions() const {
	return mPositions;
}

const std::vector<glm::vec3>& PointCloud::GetColors() const {
	return mColors;
}

Box3D PointCloud::GetBoundingBox() const {
	return mBoundingBox;
}

size_t PointCloud::GetCount() const {
	return mPositions.size();
}


void PointCloud::SetPointColor(const glm::vec3& color) {
	size_t count = GetCount();
	mColors = vector<vec3>(GetCount(), color);
	NotifyObservers();
}


// methods
void PointCloud::SetPosition(const vec3& position) {
	this->transfrom.position = position;
}

void PointCloud::SetPointColorGradient(const glm::vec3& topColor, const glm::vec3& bottomColor) {
	float minY = mBoundingBox.min.y;
	float maxY = mBoundingBox.max.y;

	if (maxY - minY < 1e-6f) {
		mColors.assign(GetCount(), topColor);
		return;
	}

	// according to the y position to assign gradient color 

	for (size_t i = 0; i < mPositions.size(); i++) {
		float t = (mPositions[i].y - minY) / (maxY - minY);
		vec3 color = bottomColor * (1.0f - t) + topColor * t;
		mColors[i] = color;
	}
	NotifyObservers();
}

void PointCloud::Flip(int axisMask) {
	const bool flipX = axisMask & X_AXIS;
	const bool flipY = axisMask & Y_AXIS;
	const bool flipZ = axisMask & Z_AXIS;
	for (auto& pos : mPositions) {
		if (flipX) pos.x = -pos.x;
		if (flipY) pos.y = -pos.y;
		if (flipZ) pos.z = -pos.z;
	}
	CalcProperties();
	AlignToOrigin();
	NotifyObservers();
}

void PointCloud::Swap(int axisMask) {
	if (axisMask == (X_AXIS | Y_AXIS))
		for (auto& pos : mPositions) std::swap(pos.x, pos.y);
	else if (axisMask == (X_AXIS | Z_AXIS))
		for (auto& pos : mPositions) std::swap(pos.x, pos.z);
	else if (axisMask == (Y_AXIS | Z_AXIS))
		for (auto& pos : mPositions) std::swap(pos.y, pos.z);
	else {
		Logger::Warn("Swap requires exactly two distinct axes.");
	}
	CalcProperties();
	NotifyObservers();
}

void PointCloud::Translate(const vec3& offset) {
	for (auto& p : mPositions) p += offset;
	
	mBoundingBox.min += offset;
	mBoundingBox.max += offset;
	NotifyObservers();
}

void PointCloud::AlignToOrigin() {
	vec3 center = mBoundingBox.GetCenter();
	Translate(-vec3(center.x, 0, center.z));
}

void PointCloud::CheckPointCountMatched() {
	if (mPositions.size() != mColors.size()) {
		Logger::Error("The number of positions and colors do not match.");
		throw std::runtime_error("The number of positions and colors do not match.");
	}
}


void PointCloud::CalcProperties() {
	// calc center
	vec3 maxP(-std::numeric_limits<float>::infinity());
	vec3 minP(std::numeric_limits<float>::infinity());

	for (const auto& p : mPositions) {
		maxP = glm::max(maxP, p);
		minP = glm::min(minP, p);
	}
	mBoundingBox = Box3D(minP, maxP);
}

void PointCloud::LoadFromXYZ(const string& filename) {
	ifstream inputFile(filename);
	if (!inputFile.is_open()) {
		Logger::Error("Failed to open input file \"{}\"\n", filename);
		std::cin.get();
	}
	float x, y, z;
	while (inputFile >> x >> y >> z) {
		mPositions.emplace_back(x, y, z);
	}
	inputFile.close();
}

void PointCloud::LoadFromPLY(const string& filename) {

	ifstream file(filename, std::ios::binary);
	if (!file.is_open()) {
		Logger::Error("Failed to open input file \"{}\"", filename);
		std::cin.get();
		return;
	}
	bool isHeaderEnded = false;
	string Line;
	size_t count = 0;

	while (getline(file, Line)) {
		if (Line.find("element vertex") != string::npos) {
			count = stoul(Line.substr(Line.find_last_of(" ") + 1));
		}
		else if (Line == "end_header") {
			isHeaderEnded = true;
			break;
		}
	}
	if (!isHeaderEnded) {
		std::print("[ERROR] Invalid .ply file {}\n", filename);
		throw std::runtime_error("Invalid .ply file");
	}
	ReadFile_doublePosition_ucharColor(file, count);

}

void PointCloud::ReadFile_doublePosition_ucharColor(std::ifstream& file, size_t count) {
	mPositions.reserve(count);
	double x = 0, y = 0, z = 0;		// position
	unsigned char r = 0, g = 0, b = 0;	// color
	for (size_t i = 0; i < count; i++) {
		file.read(reinterpret_cast<char*>(&x), sizeof(double));
		file.read(reinterpret_cast<char*>(&y), sizeof(double));
		file.read(reinterpret_cast<char*>(&z), sizeof(double));
		file.read(reinterpret_cast<char*>(&r), sizeof(unsigned char));
		file.read(reinterpret_cast<char*>(&g), sizeof(unsigned char));
		file.read(reinterpret_cast<char*>(&b), sizeof(unsigned char));
		// mPositions.emplace_back(x, y, z);
		mPositions.emplace_back(x, z, y);
		mColors.emplace_back(r / 255.0f, g / 255.0f, b / 255.0f);
	}
}

void PointCloud::ReadFile_floatPosition_floatNormal_ucharColor(std::ifstream& file, size_t count) {
	mPositions.reserve(count);
	float x = 0, y = 0, z = 0;		// position
	float nx = 0, ny = 0, nz = 0;	// normal
	unsigned char r = 0, g = 0, b = 0;	// color
	for (size_t i = 0; i < count; i++) {
		file.read(reinterpret_cast<char*>(&x), sizeof(float));
		file.read(reinterpret_cast<char*>(&y), sizeof(float));
		file.read(reinterpret_cast<char*>(&z), sizeof(float));
		file.read(reinterpret_cast<char*>(&nx), sizeof(float));
		file.read(reinterpret_cast<char*>(&ny), sizeof(float));
		file.read(reinterpret_cast<char*>(&nz), sizeof(float));
		file.read(reinterpret_cast<char*>(&r), sizeof(unsigned char));
		file.read(reinterpret_cast<char*>(&g), sizeof(unsigned char));
		file.read(reinterpret_cast<char*>(&b), sizeof(unsigned char));
		mPositions.emplace_back(x, y, z);
		//normals.emplace_back(nx, ny, nz);
		mColors.emplace_back(r, g, b);
	}
}

string PointCloud::SaveXYZ(const std::string& filename) const {
	ofstream outFile(filename);
	if (!outFile.is_open()) {
		Logger::Error("Unable to open file {}\n", filename);
		std::cin.get();
		return "";
	}
	for (const auto& p : mPositions) {
		outFile << p.x << " " << p.y << " " << p.z << endl; // xyz is good
	}
	outFile.close();
	Logger::Info("{} file write out.", filename);
	return fs::absolute(filename).string();
}

string PointCloud::SavePLY(const std::string& filename) const {
	// todo
	return fs::absolute(filename).string();
}

string PointCloud::SaveXZY(const std::string& filename) const {
	fs::path path = fs::current_path() / filename;
	ofstream outFile(path);
	if (!outFile.is_open()) {
		Logger::Error("Unable to open file {}\n", filename);
		std::cin.get();
		return "";
	}
	for (const auto& p : mPositions) {
		outFile << p.x << " " << p.z << " " << p.y << endl;
	}
	outFile.close();
	Logger::Info("{} file write out.", filename);
	return fs::absolute(filename).string();
}
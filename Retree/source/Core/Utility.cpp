#include "pch.h"
#include "Core/Utility.h"

using std::vector;
using std::string;
namespace  fs = std::filesystem;

HSV RGB2HSV(float r, float g, float b) {
    float maxV = std::max({ r, g, b });
    float minV = std::min({ r, g, b });
    float delta = maxV - minV;

    HSV hsv;

    // Hue calculation
    if (delta == 0) {
        hsv.H = 0;
    }
    else if (maxV == r) {
        hsv.H = 60.0f * fmodf(((g - b) / delta), 6.0f);
    }
    else if (maxV == g) {
        hsv.H = 60.0f * (((b - r) / delta) + 2.0f);
    }
    else { // max_val == b
        hsv.H = 60.0f * (((r - g) / delta) + 4.0f);
    }

    if (hsv.H < 0.0f)hsv.H += 360.0f;

    // Saturation
    hsv.S = (maxV == 0.0f) ? 0.0f : delta / maxV;

    // Value
    hsv.V = maxV;
    return hsv;
}

bool IsGreenLeave(float r, float g, float b) {
    return false;
}

std::string PLYtoXYZ(const string& filename, const string& newName) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        Logger::Error("Failed to open input file \"{}\"", filename);
        std::cin.get();
    }
    bool isHeaderEnded = false;
    string Line;
    size_t count = 0;

    while (getline(file, Line)) {
        if (Line.find("element vertex") != string::npos) {
            count = std::stoul(Line.substr(Line.find_last_of(" ") + 1));
            std::cout << "veretex count : " << count << std::endl;
        }
        else if (Line == "end_header") {
            isHeaderEnded = true;
            break;
        }
    }
    if (!isHeaderEnded) {
        Logger::Error("Invalid .ply file \"{}\"", filename);
        throw std::runtime_error("Invalid .ply file");
    }

    vector<glm::vec3> positions;
    vector<glm::vec3> colors;

    positions.reserve(count);
    colors.reserve(count);

    for (size_t i = 0; i < count; i++) {
        float x, y, z, nx, ny, nz;
        unsigned char r, g, b, a;
        file.read(reinterpret_cast<char*>(&x), sizeof(float));
        file.read(reinterpret_cast<char*>(&y), sizeof(float));
        file.read(reinterpret_cast<char*>(&z), sizeof(float));
        file.read(reinterpret_cast<char*>(&r), sizeof(unsigned char));
        file.read(reinterpret_cast<char*>(&g), sizeof(unsigned char));
        file.read(reinterpret_cast<char*>(&b), sizeof(unsigned char));
        file.read(reinterpret_cast<char*>(&a), sizeof(unsigned char));
        positions.emplace_back(x, y, z);
        colors.emplace_back(r, g, b);
    }
    // create xyz with only position information

    std::ofstream outFile(newName);
    if (!outFile.is_open()) {
        Logger::Error("Unable to create file \"{}\"", newName);
        std::cin.get();
    }
    for (const auto& p : positions) {
        outFile << p.x << " " << p.y << " " << p.z << '\n';
    }
    outFile.close();
    Logger::Info("PLY to XYZ done!");
    return fs::absolute(newName).string();
}



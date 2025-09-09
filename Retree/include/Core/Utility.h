#pragma once
#include <string>
#include <glm/glm.hpp>
#include <tuple>
#include <vector>

struct HSV {
	float H;	// hue [0,360]
	float S;	// saturation [0,1]
    float V;	// value [0,1]
};
HSV RGB2HSV(float r, float g, float b);
bool IsGreenLeave(float r, float g, float b);

class Utility {
public:
    
    // for Extension Adtree usage
	bool ParseAdtreePLY(const std::string& filename) {
	}
};
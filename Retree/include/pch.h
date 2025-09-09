#pragma once
// core
#include "Logger.hpp"
#include "Core/Color.h"

// std
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <random>
#include <print>
#include <format>
#include <limits>
#include <utility>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <stack>
#include <unordered_set>
#include <chrono>
#include <iomanip>
#include <tuple>
#include <functional>


// for multu-thread
#include <thread>
#include <atomic>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/quaternion.hpp>  
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>  
#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

// stb lib
#include <stb_image.h>
#include <stb_image_write.h>

// glad
#include <glad/glad.h>

// imgui
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl3.h>
#include <imgui/imgui_impl_opengl3.h>

// booster
#include <boost/graph/adjacency_list.hpp>
#include <boost/functional/hash.hpp>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


namespace fs = std::filesystem;

using std::vector;
using std::string;
using std::stack;
using std::queue;
using std::unordered_map;
using std::unordered_set;
using std::cout;
using std::endl;

using glm::vec3;
using glm::vec4;
using glm::mat4;


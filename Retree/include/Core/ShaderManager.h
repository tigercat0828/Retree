#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Core/Shader.h"
using std::shared_ptr;
using std::unordered_map;
using std::string;


class ShaderManager {
private:
    inline static unordered_map<string, shared_ptr<Shader>> Shaders;
public:
    static void LoadAll();
    static void Load(const string& filename);
    static shared_ptr<Shader> Get(const string& name);
    static void Release();
};
#pragma once
#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include "Core/AssimpModel.h"
#include "Core/Texture.h"
using std::shared_ptr;
using std::string;
using std::unordered_map;

class ResourceManager {
private:
    inline static unordered_map<string, shared_ptr<Texture>> TextureMap;
    inline static unordered_map<string, shared_ptr<AssimpModel>> ModelMap;
public:
    static void LoadTexture(const string& filename);
    static void LoadModel(const string& filename);
    
    static shared_ptr<Texture> GetTexture(const string& name);
    static shared_ptr<AssimpModel> GetModel(const string& name);

     // static void LoadAll();
};
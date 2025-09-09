#include "pch.h"
#include "Core/ResourceManager.h"

void ResourceManager::LoadTexture(const string& filename) {
    if (TextureMap.find(filename) == TextureMap.end()) {
        TextureMap[filename] = make_shared<Texture>(filename);
    }
}

void ResourceManager::LoadModel(const string& filename) {
    if (ModelMap.find(filename) == ModelMap.end()) {
        ModelMap[filename] = make_shared<AssimpModel>(filename);
    }
}

shared_ptr<Texture> ResourceManager::GetTexture(const string& name) {
    if (TextureMap.find(name) == TextureMap.end()) {
        Logger::Error("Need to load texture {} first!\n", name);
        throw std::runtime_error("Need to load shader first!");
    }
    return TextureMap[name];
}

shared_ptr<AssimpModel> ResourceManager::GetModel(const string& name) {
    if (ModelMap.find(name) == ModelMap.end()) {
        Logger::Error("Need to load model {} first!\n", name);
        throw std::runtime_error("Need to load model first!");
    }
    return ModelMap[name];
}


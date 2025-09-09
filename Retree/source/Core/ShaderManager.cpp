#include "pch.h"

#include "Core/ShaderManager.h"

void ShaderManager::LoadAll() {
    const string ShaderDir = "Assets/Shaders/";
    for (const auto& entry : fs::recursive_directory_iterator(ShaderDir)) {
        if (entry.is_regular_file()) {
            fs::path relativePath = fs::relative(entry.path(), ShaderDir);
            relativePath.replace_extension();   // remove extension
            string name = relativePath.generic_string();    // solve '\\' and '/' confliction
            ShaderManager::Load(name);
        }
    }
}

void ShaderManager::Load(const string& filename) {
    if (Shaders.find(filename) == Shaders.end()) {
        Shaders[filename] = make_shared<Shader>(filename);
    }
}

shared_ptr<Shader> ShaderManager::Get(const string& name) {

    if (Shaders.find(name) == Shaders.end()) {
        Logger::Error("Need to load shader {} first!\n", name);
        throw std::runtime_error("Need to load shader first!");
    }
    return Shaders[name];
}

void ShaderManager::Release() {
    for (auto& pair : Shaders) pair.second->Delete();
    Shaders.clear();
}

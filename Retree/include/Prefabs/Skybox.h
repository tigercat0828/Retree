#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Core/Shader.h"
#include "Core/GameObject.h"
using GLuint = unsigned int;
using std::string;
using std::shared_ptr;
class Camera;

class Skybox : public GameObject{
private:
    GLuint mTextureID = 0;
    GLuint mVAO;
    GLuint mVBO;

    shared_ptr<Shader> pShader;
public:
    Skybox(const string& skyboxfolder = "skybox");
    ~Skybox() = default;

    void OnDraw(Camera& camera);
    GLuint GetTextureID() const { return mTextureID; }

private:
    void SetupGL();
    void CleanGL();
    void LoadTexture(const string& skyboxfolder);
public:
    static float SkyboxVertices[108];
};




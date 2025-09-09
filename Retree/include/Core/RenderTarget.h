#pragma once
#include <glm/glm.hpp>
#include <string>
using std::string;
using glm::vec2;
using GLuint = unsigned int;

class RenderTarget {
public:
    RenderTarget() = default;
    RenderTarget(int width, int height);
private:    // property
    GLuint mFBO{ 0 };
    GLuint mRBO{ 0 };
    GLuint mTextureID{ 0 };
    int mWidth { 800 };
    int mHeight{ 600 };
public:     // getter-setter
    void GetSize(int& width, int& height) const;
    vec2 GetSize() const;
    GLuint GetTextureID() const;
public:     // method
    void Create();
    void Bind() const;
    void Unbind() const;
    void Rescale(int width, int height);
    string SaveToFile();
    string SaveToFile(const string& filename);
private:
    void SaveFrameBuffer(const string& fullPath);
};


#include "pch.h"
#include "Core/RenderTarget.h"
#include "Core/FileHelper.h"
RenderTarget::RenderTarget(int width, int height) : mWidth(width), mHeight(height) { Create(); }

void RenderTarget::GetSize(int& width, int& height) const {
    width = mWidth;
    height = mHeight;
}

glm::vec2 RenderTarget::GetSize() const {
    return glm::vec2(mWidth, mHeight);
}

GLuint RenderTarget::GetTextureID() const { return mTextureID; }

void RenderTarget::Create() {
    glGenFramebuffers(1, &mFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);

    glGenRenderbuffers(1, &mRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Logger::Warn("Framebuffer is not complete!");
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void RenderTarget::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
}

void RenderTarget::Unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::Rescale(int width, int height) {
    mWidth = width;
    mHeight = height;
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, mRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRBO);
}

std::string RenderTarget::SaveToFile() {
    std::string timestamp = FileHelper::GetTimestampFilename();
    std::string fullPath = "Screenshots/" + timestamp + ".png";
    std::filesystem::create_directories("Screenshots");
    SaveFrameBuffer(fullPath);
    return fullPath;
}

std::string RenderTarget::SaveToFile(const std::string& filename) {
    std::string fullPath = "Screenshots/" + filename + ".png";
    SaveFrameBuffer(fullPath);
    return fullPath;
}

void RenderTarget::SaveFrameBuffer(const std::string& fullPath) {

    glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    unsigned char* pixels = new unsigned char[mWidth * mHeight * 4];
    glReadPixels(0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Flip vertically
    for (int y = 0; y < mHeight / 2; ++y) {
        for (int x = 0; x < mWidth * 4; ++x) {
            std::swap(pixels[y * mWidth * 4 + x], pixels[(mHeight - 1 - y) * mWidth * 4 + x]);
        }
    }

    stbi_write_png(fullPath.c_str(), mWidth, mHeight, 4, pixels, mWidth * 4);

    delete[] pixels;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

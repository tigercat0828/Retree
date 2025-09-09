#include "pch.h"
#include "Core/Texture.h"

Texture::Texture(const std::string& filename) {
	Name = filename;

	glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);

    fs::path path = fs::current_path() /  "Assets" / "Textures" / filename;
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0); // keep original channels

    if (data) {
        unsigned char* processedData = data;
        int finalChannels = channels;

        // Convert grayscale (1-channel) to RGB (3-channel)
        if (channels == 1) {
            finalChannels = 3;
            processedData = new unsigned char[width * height * finalChannels];
            for (int i = 0; i < width * height; ++i) {
                unsigned char gray = data[i];
                processedData[i * 3 + 0] = gray; // R
                processedData[i * 3 + 1] = gray; // G
                processedData[i * 3 + 2] = gray; // B
            }
            Logger::Debug("Grayscale image converted to RGB");
        }

        GLenum pixelFormat = GL_RGB;
        GLenum internalFormat = GL_RGB;

        if (finalChannels == 3) {
            pixelFormat = internalFormat = GL_RGB;
        }
        else if (finalChannels == 4) {
            pixelFormat = internalFormat = GL_RGBA;
        }

        int stride = width * finalChannels;
        if (stride % 4 != 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            //Logger::Debug("Image offset fixed");
        }
        else {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        }

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            internalFormat,
            width, height,
            0,
            pixelFormat,
            GL_UNSIGNED_BYTE,
            processedData
        );
        glGenerateMipmap(GL_TEXTURE_2D);
        Logger::Info("Success to load texture \"{}\", size {} x {}, channels {}", Name, width, height, finalChannels);
        Logger::Debug("channel {}", finalChannels);

        // Clean up converted buffer if grayscale
        if (processedData != data) {
            delete[] processedData;
        }
        stbi_image_free(data);
    }
    else {
        Logger::Warn("Failed to load texture \"{}\"", Name);
    }
    stbi_set_flip_vertically_on_load(false);
}


// method
void Texture::Bind() {
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::GetSize(int& width, int& height) const {
	width = this->width;
	height = this->height;
}

std::string Texture::GetName() const { return Name;  }

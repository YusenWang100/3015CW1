#pragma once
#include <glad/glad.h>

#include <string>
#include <vector>

class Texture {
    unsigned char* data{};
    int width;
    int height;
    std::string file;
    bool isLinear;

    void LoadFile();

public:
    GLuint texture;

    Texture() {}
    Texture(const char* file, bool isLinear);

    ~Texture() { Destroy(); }

    Texture(Texture const&) = delete;
    Texture& operator=(Texture const&) = delete;


    void Create();
    void Create(int w, int h, GLenum internalFormat, GLenum format, GLenum type, GLenum wrapMode);
    void Bind(int index);

    void Destroy();
};
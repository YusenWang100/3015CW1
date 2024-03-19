#include "scene0/texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "helper/stb/stb_image.h"

Texture::Texture(const char* file, bool isLinear) : file(file), isLinear(isLinear) {}
void Texture::Create(int w, int h, GLenum internalFormat, GLenum format, GLenum type, GLenum wrapMode) {
    width = w;
    height = h;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::LoadFile() {
    int comp;
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load(file.c_str(), &width, &height, &comp, 4);
    if (!data) fprintf(stderr, "failed to load image %s\n", file);
}
void Texture::Create() {
    LoadFile();
    GLenum format = GL_SRGB8_ALPHA8;
    if (isLinear) format = GL_RGBA8;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::Bind(int index) {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture);
}
void Texture::Destroy() {
    if (data) stbi_image_free(data);
    data = 0;
    glDeleteTextures(1, &texture);
}
#pragma once
#include <vector>

#include "glm/glm.hpp"
#include "scene0/texture.h"

class Skybox {
    GLuint vao;
    GLuint vbo;

    GLuint tex;
    int width;
    int height;

public:
    /**
     * @brief order
     * +x
     * -x
     * +y
     * -y
     * +z
     * -z
     */
    std::vector<std::string> faces;

    void Init();
    void Draw();
};
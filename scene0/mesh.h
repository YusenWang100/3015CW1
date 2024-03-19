#pragma once
#include <glad/glad.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "scene0/texture.h"

struct Transform {
    Transform* parent{};
    std::vector<Transform*> children;

    glm::vec3 translation{};
    glm::quat rotation = glm::quat();
    glm::vec3 scale{1, 1, 1};

    glm::mat4 matrix = glm::mat4(1);

    GLuint transformUBO{};

    void Init()
    {
        glGenBuffers(1, &transformUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, transformUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), &matrix, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void Update() {
        if (!transformUBO) Init();

        glm::mat4 m = glm::mat4(1);
        if (parent) m = parent->matrix;
        matrix = m * glm::scale(glm::translate(glm::mat4(1), translation) * glm::mat4_cast(rotation), scale);

        glBindBuffer(GL_UNIFORM_BUFFER, transformUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &matrix);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        for (auto e : children) e->Update();
    }
    void Bind(int index) { glBindBufferBase(GL_UNIFORM_BUFFER, index, transformUBO); }
};

struct AABB {
    glm::vec3 pmin = glm::vec3(FLT_MAX);
    glm::vec3 pmax = glm::vec3(-FLT_MAX);
    glm::vec3 Center() const { return (pmin + pmax) / 2.f; }
    glm::vec3 Size() const { return pmax - pmin; }
    void Merge(glm::vec3 const& p) {
        pmin = glm::min(pmin, p);
        pmax = glm::max(pmax, p);
    }
    void Merge(AABB const& other) {
        pmin = glm::min(pmin, other.pmin);
        pmax = glm::max(pmax, other.pmax);
    }
};

struct Material {
    GLuint ubo;

    struct UBOData {
        glm::vec4 diffuseColor;
        glm::vec4 specularColor;
        glm::vec4 ambient;
        glm::vec3 emissionColor{};
        float shininess;
        int enableSpecular{1};
        int onlyEmission{0};
        int enableToonShading{0};
        int useNormalMap = 0;
    } uboData{};

    Texture* diffuseTex{};
    Texture* normalTex{};

    void Init();
    void Bind(int index);
    void Update();
};

struct Primitive {
    GLenum topology;

    uint32_t firstVertex;  // add to index of vertex
    uint32_t vertexCount;

    // draw index
    uint32_t firstIndex;
    uint32_t indexCount;
};

class Mesh {
    template <typename T>
    GLuint CreateBuffer(std::vector<T> const& data) {
        if (data.empty()) return 0;
        GLuint buffer{};
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return buffer;
    }

public:
    std::string name;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    std::vector<glm::u8vec4> colors;

    // Eigen::Vector3
    std::vector<unsigned int> indices;

    std::vector<Primitive> primitives;

    AABB aabb;

    GLuint vao;
    GLuint vboPosition{};
    GLuint vboNormal{};
    GLuint vboTexcoord{};
    GLuint vboTangent{};
    GLuint vboBitangent{};
    // GLuint vboColor{};

    GLuint ibo{};

    Material* material{};

    Mesh() = default;
    ~Mesh();

    Mesh(Mesh const&) = delete;
    Mesh& operator=(Mesh const&) = delete;

    void Init();

    void Draw();

    // static void BuildAxis(Mesh& mesh, float length = 1.);
    // static void BuildTriangle(Mesh& mesh, glm::vec3 const& a, glm::vec3 const& b, glm::vec3 const& c);
    // static void BuildGround(Mesh& mesh, float x0, float y0, float w, float h, int segCountX, int segCountY);
    // static void BuildAABB(Mesh& mesh, glm::vec3 const& a, glm::vec3 const& b);
    // static void BuildBox(Mesh& mesh, glm::vec3 const& a, glm::vec3 const& b);
    // static void BuildDisk(Mesh& mesh, glm::vec3 const& c, float radius);
    // static void BuildGrid(Mesh& mesh, float x0, float y0, float w, float h, int segCount);
    // static void BuildSphere(Mesh& mesh, float r, int latitudeSegCount, int longitudeSegCount);
};

class Model {
public:
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
    std::vector<std::unique_ptr<Material>> materials;

    Transform transform;

    void Init();

    void Update();

    void Draw();
};

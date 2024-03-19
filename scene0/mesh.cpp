#include "scene0/mesh.h"

void Material::Init() {
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(UBOData), &uboData, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Material::Bind(int index) {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo);
    if (diffuseTex) diffuseTex->Bind(0);
    if (normalTex) normalTex->Bind(1);
}
void Material::Update() {
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(UBOData), &uboData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vboPosition);
    glDeleteBuffers(1, &vboNormal);
    glDeleteBuffers(1, &vboTexcoord);
    glDeleteBuffers(1, &vboTangent);
    glDeleteBuffers(1, &vboBitangent);
    // glDeleteBuffers(1, &vboColor);
    glDeleteBuffers(1, &ibo);
}
void Mesh::Init() {
    // create vbos
    vboPosition = CreateBuffer(positions);
    vboNormal = CreateBuffer(normals);
    vboTexcoord = CreateBuffer(texcoords);
    vboTangent = CreateBuffer(tangents);
    vboBitangent = CreateBuffer(bitangents);
    // vboColor = CreateBuffer(colors);
    ibo = CreateBuffer(indices);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vboPosition);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (!normals.empty()) {
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (!texcoords.empty()) {
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, vboTexcoord);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (!tangents.empty()) {
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, vboTangent);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    if (!bitangents.empty()) {
        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, vboBitangent);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }

    glBindVertexArray(0);
}
void Mesh::Draw() {
    if (material) material->Bind(3);
    glBindVertexArray(vao);
    for (auto &&e : primitives) {
        if (e.indexCount > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glDrawRangeElements(e.topology, e.firstIndex, e.indexCount - e.firstIndex, e.indexCount, GL_UNSIGNED_INT,
                                0);
        } else
            glDrawArrays(e.topology, e.firstVertex, e.vertexCount);
    }
    glBindVertexArray(0);
}
void Model::Init() {
    for (auto &&e : textures) e.second->Create();
    for (auto &&e : materials) e->Init();
    for (auto &&e : meshes) e->Init();
    transform.Init();
}
void Model::Update() { transform.Update(); }
void Model::Draw() {
    transform.Bind(2);
    for (auto &&e : meshes) e->Draw();
}
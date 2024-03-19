#pragma once
#include <filesystem>
#include <iostream>

#include "scene0/mesh.h"

#define OBJLOADER_IMPLEMENTATION
bool loadModelObj(Model& model, const char* inputfile);

#ifdef OBJLOADER_IMPLEMENTATION

#define TINYOBJLOADER_IMPLEMENTATION
#include "helper/tiny_obj_loader.h"

void calcTB(glm::vec3 pos[3], glm::vec2 uv[3], glm::vec3& T, glm::vec3& B) {
    auto e1 = pos[1] - pos[0];
    auto e2 = pos[2] - pos[0];
    auto duv1 = uv[1] - uv[0];
    auto duv2 = uv[2] - uv[0];
    glm::mat2 mat_uv(duv1, duv2);
    glm::mat2x3 E(e1, e2);
    auto a = E * glm::inverse(mat_uv);
    T = glm::normalize(a[0]);
    B = glm::normalize(a[1]);
}

bool loadModelObj(Model& model, const char* inputfile) {
    std::filesystem::path path(inputfile);
    auto parentPath = path.parent_path();

    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = parentPath.string();  // Path to material files
    reader_config.triangulate = true;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    auto loadTex = [&](std::string const& texName, bool isLinear) -> Texture* {
        Texture* ret;
        auto it = model.textures.find(texName);
        if (it == model.textures.end()) {
            ret = (model.textures.emplace(texName, std::make_unique<Texture>(texName.c_str(), isLinear)))
                      .first->second.get();
        } else
            ret = it->second.get();
        return ret;
    };

    // load materials
    for (auto& e : materials) {
        auto mat = model.materials.emplace_back(std::make_unique<Material>()).get();

        if (!e.diffuse_texname.empty()) mat->diffuseTex = loadTex(parentPath.string() + "/" + e.diffuse_texname, true);
        if (!e.normal_texname.empty()) {
            mat->normalTex = loadTex(parentPath.string() + "/" + e.normal_texname, true);
            mat->uboData.useNormalMap = 1;
        }

        mat->uboData.diffuseColor = {e.diffuse[0], e.diffuse[1], e.diffuse[2], 1};
        mat->uboData.specularColor = {e.specular[0], e.specular[1], e.specular[2], 1};
        mat->uboData.shininess = e.shininess;
        mat->uboData.ambient = {e.ambient[0], e.ambient[1], e.ambient[2], 1};
    }

    // Loop over shapes
    int curMaterialId = -2;
    Mesh* curMesh = 0;
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            auto materialId = shapes[s].mesh.material_ids[f];
            if (curMaterialId != materialId) {
                curMaterialId = materialId;
                curMesh = new Mesh();
                model.meshes.emplace_back(std::unique_ptr<Mesh>(curMesh));
                curMesh->name = shapes[s].name;
                curMesh->material = model.materials[curMaterialId].get();
            }
            //

            //
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
                curMesh->positions.emplace_back(vx, vy, vz);

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    auto N = glm::normalize(glm::vec3(nx, ny, nz));
                    curMesh->normals.emplace_back(N);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    curMesh->texcoords.emplace_back(tx, ty);
                }

                // Optional: vertex colors
                // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
            }
            index_offset += fv;
        }
    }
    glm::vec3 T, B;
    for (auto&& e : model.meshes) {
        for (size_t i = 0; i < e->positions.size() && !e->texcoords.empty(); i += 3) {
            calcTB(&e->positions[i], &e->texcoords[i], T, B);
            e->tangents.emplace_back(T);
            e->tangents.emplace_back(T);
            e->tangents.emplace_back(T);
            e->bitangents.emplace_back(B);
            e->bitangents.emplace_back(B);
            e->bitangents.emplace_back(B);
        }
    }
    for (auto&& e : model.meshes) {
        auto& primitive = e->primitives.emplace_back();
        primitive.topology = GL_TRIANGLES;
        primitive.vertexCount = e->positions.size();
    }
    model.Init();
    return true;
}

#endif
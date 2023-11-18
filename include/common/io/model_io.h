#pragma once

#include <vector>
#include <string>
#include "common/mesh_model.hxx"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

class ModelIO {
    std::string directory;

    public:
        std::vector<MeshModel> read_obj_model(std::string model_path);

    void load_material_texture(aiMaterial *material, aiTextureType type, MeshModel &model);

    MeshModel process_mesh(aiMesh *mesh, const aiScene *scene);

    void process_node(aiNode *node, const aiScene *scene, std::vector<MeshModel> &meshes);
};
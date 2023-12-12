#include "common/io/model_io.h"
#include "common/mesh_model.hxx"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

void ModelIO::load_material_texture(aiMaterial *material, aiTextureType type, MeshModel &model) {
    for (auto i = 0; i < material->GetTextureCount(type); i++) {
        aiString str;
        material->GetTexture(type, i, &str);

        std::string path = std::format("{}/{}", this->directory, std::string(str.C_Str()));
        std::cout << path << std::endl;
        if (type == aiTextureType::aiTextureType_DIFFUSE)
            model.bind_texture(path, TextureType::diffuse_texture);
        if (type == aiTextureType::aiTextureType_SPECULAR)
            model.bind_texture(path, TextureType::specular_texture);
    }
}

MeshModel ModelIO::process_mesh(aiMesh *mesh, const aiScene *scene) {
    MeshModel model;

    for (auto i = 0; i < mesh->mNumVertices; i++) {
        decltype(model.vertices)::value_type vertex;
        vertex.point = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        if (mesh->HasNormals())
            vertex.normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->mTextureCoords[0])
            vertex.texture_coord = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        model.vertices.push_back(vertex);
    }

    for (auto i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        model.faces_indices.push_back({face.mIndices[0], face.mIndices[1], face.mIndices[2]});
//        std::cout << std::format("indices {} {} {}\n", face.mIndices[0], face.mIndices[1], face.mIndices[2]);
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
    load_material_texture(material, aiTextureType::aiTextureType_DIFFUSE, model);
    load_material_texture(material, aiTextureType::aiTextureType_SPECULAR, model);

    model.transform = glm::identity<glm::mat4>();
    model.transform = glm::scale(model.transform, glm::vec3(0.1, 0.1, 0.1));
    model.transform = glm::translate(model.transform, {0, 3, 0});

    return model;
}

void ModelIO::process_node(aiNode *node, const aiScene *scene, std::vector<MeshModel> &meshes) {
    for (auto i = 0; i < node->mNumMeshes; i++) {
        auto mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }
    for (auto i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene, meshes);
    }
}

std::vector<MeshModel> ModelIO::read_obj_model(std::string model_path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(model_path, aiProcess_Triangulate | aiProcess_FlipUVs);

    std::vector<MeshModel> res;

    this->directory = model_path.substr(0, model_path.find_last_of('/'));

    process_node(scene->mRootNode, scene, res);

    return res;
}
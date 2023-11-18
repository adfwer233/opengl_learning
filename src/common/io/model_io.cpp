#include "common/io/model_io.h"
#include "common/mesh_model.hxx"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

MeshModel process_mesh(aiMesh *mesh, const aiScene *scene) {
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
    }

    model.transform = glm::identity<glm::mat4>();
    model.transform = glm::scale(model.transform, glm::vec3(0.1, 0.1, 0.1));
    return model;
}

void process_node(aiNode *node, const aiScene *scene, std::vector<MeshModel> &meshes) {
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

    process_node(scene->mRootNode, scene, res);

    return res;
}
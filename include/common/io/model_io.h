#pragma once

#include <vector>
#include <string>
#include "common/mesh_model.hxx"

class ModelIO {
    public:
        static std::vector<MeshModel> read_obj_model(std::string model_path);
};
#pragma once

#include "common/mesh_model.hxx"
#include "glm/glm.hpp"

class Mirror {
    MeshModel rect_model;

    unsigned int frame_buffer, mirror_texture, render_buffer;

    void bind_frame_buffer(int width, int height);
};
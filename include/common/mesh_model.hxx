#pragma once

#include <list>
#include <iostream>
#include <vector>

#include "half_edge.hxx"
#include "point.hxx"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TriangleVerticeIndex {
    unsigned int x, y, z;
};

struct MeshModel {

    std::vector<Point3d> vertices;
    std::vector<TriangleVerticeIndex> faces_indices;

    glm::mat4 transform;

    MeshModel() : transform(glm::mat4(1.0f)) {}

    static bool collision_test(MeshModel model1, MeshModel model2);
};
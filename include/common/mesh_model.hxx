#pragma once

#include <list>
#include <iostream>
#include <vector>

#include "half_edge.hxx"
#include "point.hxx"

struct TriangleVerticeIndex {
    unsigned int x, y, z;
};

struct MeshModel {

    std::vector<Point3d> vertices;
    std::vector<TriangleVerticeIndex> faces_indices;

    static bool collision_test(MeshModel model1, MeshModel model2);
};
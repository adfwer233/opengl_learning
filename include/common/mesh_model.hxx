#pragma once

#include <list>
#include <iostream>

#include "half_edge.hxx"
#include "point.hxx"

struct MeshModel {
    std::list<half_edge_face*> mesh;

    static bool collision_test(MeshModel model1, MeshModel model2);
};
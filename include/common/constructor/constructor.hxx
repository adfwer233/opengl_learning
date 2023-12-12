#pragma once

#include "../mesh_model.hxx"
#include "../point.hxx"

class Constructor {

public:
    static MeshModel Cubic(Point3d point1, Point3d Point2d);

    static MeshModel Sphere(Point3d center, float radius);

    static MeshModel Rectangle(glm::vec3 left_bot, glm::vec3 left_top, glm::vec3 right_bot);
};
#pragma once

#include "../mesh_model.hxx"
#include "../point.hxx"

class Constructor {
    
    static MeshModel Cubic(Point3d point1, Point3d Point2d);
    static MeshModel Sphere(Point3d center, double radius);
    
};
#pragma once

struct Point2d {
    float x, y;
};

struct Point3d {
    float x, y, z;
};

float inner_product(Point2d u, Point2d v);

Point2d normalize(Point2d vec);
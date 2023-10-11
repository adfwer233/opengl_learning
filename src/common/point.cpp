#include <cmath>

#include "common/point.hxx"

float inner_product(Point2d u, Point2d v) {
    return u.x * v.x + u.y * v.y;
}

Point2d normalize(Point2d vec) {
    float len = std::sqrt(inner_product(vec, vec));
    return Point2d{vec.x / len, vec.y / len};
}
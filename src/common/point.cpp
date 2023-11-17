#include <cmath>

#include "common/point.hxx"
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

float inner_product(Point2d u, Point2d v) {
    return u.x * v.x + u.y * v.y;
}

Point2d normalize(Point2d vec) {
    float len = std::sqrt(inner_product(vec, vec));
    return Point2d{vec.x / len, vec.y / len};
}

glm::vec3 Point3d::to_glm() {
   return {x, y, z};
}

glm::vec2 Point2d::to_glm() {
    return {x, y};
}
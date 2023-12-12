#pragma once
#include <concepts>
#include "glm/glm.hpp"

struct Point2d {
    float x, y;

    glm::vec2 to_glm();
};

struct Point3d {
    float x, y, z;

    Point3d operator + (Point3d v) const {
        return Point3d{x + v.x, y + v.y, z + v.z };
    }

    Point3d operator - (Point3d v) const {
        return Point3d{ x - v.x, y - v.y, z - v.z };
    }

    template<typename T>
    Point3d operator * (T c)  requires std::is_integral_v<T> || std::is_floating_point_v<T> {
        return Point3d { x * c, y * c, z * c};
    }

    template<typename T>
    Point3d operator / (T c) const requires requires (T a) { x / a; } {
        return Point3d { x / c, y / c, z / c};
    }

    static Point3d outer_product(Point3d a, Point3d b) {
        return Point3d { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
    }

    glm::vec3 to_glm();

    static float inner_product(Point3d &u, Point3d &v);
};

float inner_product(Point2d u, Point2d v);

Point2d normalize(Point2d vec);
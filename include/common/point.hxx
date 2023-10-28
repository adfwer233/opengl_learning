#pragma once
#include <concepts>

struct Point2d {
    float x, y;
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
};

float inner_product(Point2d u, Point2d v);

Point2d normalize(Point2d vec);
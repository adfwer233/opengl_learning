#include <cmath>
#include <numbers>
#include <ranges>

#include "common/constructor/constructor.hxx"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

MeshModel Constructor::Cubic(Point3d point1, Point3d point2) {
    MeshModel model;

    auto a = point2.x - point1.x;
    auto b = point2.y - point1.y;
    auto c = point2.z - point1.z;

    std::vector<Point3d> vertice {
        Point3d{point1.x, point1.y, point1.z},
        Point3d{point1.x + a, point1.y, point1.z},
        Point3d{point1.x, point1.y + b, point1.z},
        Point3d{point1.x + a, point1.y + b, point1.z},
        Point3d{point1.x, point1.y, point1.z + c},
        Point3d{point1.x + a, point1.y, point1.z + c},
        Point3d{point1.x, point1.y + b, point1.z + c},
        Point3d{point1.x + a, point1.y + b, point1.z + c},
    };

    std::vector<TriangleVerticeIndex> index {
        {0, 1, 2}, {1, 2, 3},   // bottom
        {4, 5, 6}, {5, 6, 7},   // top
        {0, 2, 4}, {0, 6, 2},   // left
        {1, 5, 3}, {3, 5, 7},   // right
        {0, 1, 4}, {1, 4, 5},   // front
        {2, 3, 6}, {3, 6, 7}    // back
    };

    model.vertices = vertice;
    model.faces_indices = index;

    model.transform = glm::mat4(1.0f);

    return model;
}

/*
 * Construct a sphere:
 *      Step1: Generate the mesh of unit sphere
 *      Step2: Calculate the transformation of target
 */
MeshModel Constructor::Sphere(Point3d center, double radius) {
    using namespace std::numbers;

    MeshModel model;

    constexpr int theta_segments = 50;
    constexpr int phi_segments = 50;

    // Generate Vertices
    for (int i: std::views::iota(0, theta_segments + 1)) {
        for (int j: std::views::iota(0, phi_segments + 1)) {
            double theta = pi * (1.0f * i / theta_segments);
            double phi = 2 * pi * (1.0f * j / theta_segments);

            float x = std::sin(theta) * std::cos(phi);
            float y = std::sin(theta) * std::sin(phi);
            float z = std::cos(theta);

            model.vertices.push_back({x, y, z});
        }
    }

    // Generate Indices
    for (unsigned int i = 0; i < theta_segments; i++) {
        for (unsigned int j = 0; j < phi_segments; j++) {
            auto ind = [&](auto x, auto y) {
                return x * (theta_segments + 1) + y;
            };
            model.faces_indices.push_back({ind(i, j), ind(i + 1, j), ind(i + 1, j + 1)});
            model.faces_indices.push_back({ind(i, j), ind(i + 1, j + 1), ind(i, j + 1)});
        }
    }


    model.transform = glm::translate(model.transform, glm::vec3(center.x, center.y, center.z));
    model.transform = glm::scale(model.transform, glm::vec3(radius, radius, radius));

    return model;
}
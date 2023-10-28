#include <cmath>
#include <numbers>
#include <ranges>

#include "common/constructor/constructor.hxx"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

MeshModel Constructor::Cubic(Point3d point1, Point3d point2) {
    MeshModel model;

    // generate tri mesh for a rectangle

    constexpr int segment = 50;

    auto mesh_rectangle = [&](Point3d base, Point3d vec_x, Point3d vec_y, Point3d norm) {
        decltype(model.vertices) vertices;
        decltype(model.faces_indices) indices;

        auto dx = vec_x / segment;
        auto dy = vec_y / segment;

        for (int i: std::views::iota(0, segment + 1)) {
            for (int j: std::views::iota(0, segment + 1)) {
                auto v = base + dx * i + dy * j;
                vertices.push_back({v, norm});
            }
        }

        for (unsigned int i = 0; i < segment; i++) {
            for (unsigned int j = 0; j < segment; j++) {
                auto ind = [&](auto x, auto y) -> unsigned int {
                    return x * (segment + 1) + y + model.vertices.size();
                };
                indices.push_back({ind(i, j), ind(i + 1, j), ind(i + 1, j + 1)});
                indices.push_back({ind(i, j), ind(i + 1, j + 1), ind(i, j + 1)});
            }
        }

        std::cout << model.vertices.size() << ' ' << vertices.size() << ' ' << model.faces_indices.size() << std::endl;

        std::ranges::copy(vertices, std::back_inserter(model.vertices));
        std::ranges::copy(indices, std::back_inserter(model.faces_indices));

    };

    auto a = point2.x - point1.x;
    auto b = point2.y - point1.y;
    auto c = point2.z - point1.z;

    mesh_rectangle({point1.x, point1.y, point1.z}, {a, 0, 0}, {0, b, 0}, {0, 0, -1});
    mesh_rectangle({point1.x, point1.y, point1.z}, {a, 0, 0}, {0, 0, c}, {0, -1, 0});
    mesh_rectangle({point1.x, point1.y, point1.z}, {0, b, 0}, {0, 0, c}, {-1, 0, 0});
    mesh_rectangle({point1.x + a, point1.y, point1.z}, {0, b, 0}, {0, 0, c}, {1, 0, 0});
    mesh_rectangle({point1.x, point1.y + b, point1.z}, {a, 0, 0}, {0, 0, c}, {0, 1, 0});
    mesh_rectangle({point1.x, point1.y, point1.z + c}, {a, 0, 0}, {0, b, 0}, {0, 0, 1});

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

            model.vertices.push_back({{x, y, z}, {x, y, z}});
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
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

    constexpr int segment = 3;

    auto mesh_rectangle = [&](glm::vec3 base, glm::vec3 vec_x, glm::vec3 vec_y, glm::vec3 norm) {
        decltype(model.vertices) vertices;
        decltype(model.faces_indices) indices;

        auto dx = vec_x / float(segment);
        auto dy = vec_y / float(segment);

        for (int i: std::views::iota(0, segment + 1)) {
            for (int j: std::views::iota(0, segment + 1)) {
                auto v = base + dx * float(i) + dy * float(j);
                glm::vec2 texture{1.0f * i / segment, 1.0f * j / segment};
                vertices.push_back({v, norm, texture});
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

    model.box = AxisAlignedBoundingBox(point1, point2);

    return model;
}

/*
 * Construct a sphere:
 *      Step1: Generate the mesh of unit sphere
 *      Step2: Calculate the transformation of target
 */
MeshModel Constructor::Sphere(Point3d center, float radius) {
    using namespace std::numbers;

    MeshModel model;

    constexpr int theta_segments = 50;
    constexpr int phi_segments = 50;

    // Generate Vertices
    for (int i: std::views::iota(0, theta_segments + 1)) {
        for (int j: std::views::iota(0, phi_segments + 1)) {
            double theta = pi * (1.0f * i / theta_segments);
            double phi = 2 * pi * (1.0f * j / phi_segments);

            float x = std::sin(theta) * std::cos(phi);
            float y = std::sin(theta) * std::sin(phi);
            float z = std::cos(theta);

            model.vertices.push_back({{x, y, z}, {x, y, z}, {1.0f * i / theta_segments, 1.0f * j / phi_segments }});
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

    model.box = AxisAlignedBoundingBox({-1, -1, -1}, {1, 1, 1});
    return model;
}

MeshModel Constructor::Rectangle(glm::vec3 left_bot, glm::vec3 left_top, glm::vec3 right_bot) {
    auto vec1 = right_bot - left_bot;
    auto vec2 = left_top - left_bot;

    auto normal = glm::cross(vec1, vec2);

    auto right_top = left_bot + vec1 + vec2;

    MeshModel model;
    model.vertices = {
            {left_bot, normal, {0, 0}},
            {left_top, normal, {1, 0}},
            {right_bot, normal, {0, 1}},
            {right_top, normal, {1, 1}},
    };

    model.faces_indices = {
            {0, 1, 2},
            {1, 2, 3}
    };

    model.transform = glm::identity<glm::mat4>();
    model.box = AxisAlignedBoundingBox({left_bot.x, left_bot.y, left_bot.z}, {right_top.x, right_top.y, right_top.z});
    return model;
}
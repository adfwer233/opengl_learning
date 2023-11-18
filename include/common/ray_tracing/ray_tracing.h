#pragma once

#include <vector>
#include <array>
#include <map>

#include "common/ray_tracing/ray.h"
#include "common/camera/camera.hxx"
#include "common/mesh_model.hxx"
#include "common/io/render_output.h"

constexpr const int n = 720;
constexpr const int m = 720;

std::array<std::array<glm::vec3, m>, n> image;

bool ray_tracing_box_test (Ray ray, AxisAlignedBoundingBox box) {
    float minx, miny, minz, maxx, maxy, maxz;
    minx = box.x_range.start;
    miny = box.y_range.start;
    minz = box.z_range.start;
    maxx = box.x_range.end;
    maxy = box.y_range.end;
    maxz = box.z_range.end;

    std::vector<glm::vec3> vertices{
            {minx, miny, minz},
            {minx, maxy, minz},
            {maxx, miny, minz},
            {maxx, maxy, minz},
            {minx, miny, maxz},
            {minx, maxy, maxz},
            {maxx, miny, maxz},
            {maxx, maxy, maxz},
    };

    std::vector<std::tuple<int, int, int>> indices {
            {0, 1 ,2}, {1, 2, 3}, // minz
            {4, 5, 6}, {5, 6, 7}, // maxz
            {0, 1, 4}, {1, 4, 5}, // minx
            {2, 3, 6}, {3, 6, 7}, // maxx
            {0, 2, 4}, {2, 4, 6}, // miny
            {1, 3, 5}, {3, 5, 7}  // maxy
    };

    for (auto tup: indices) {
        auto [i, j, k] = tup;
        auto [flag, t, u, v, w] = ray.ray_triangle_intersection(vertices[i], vertices[j], vertices[k]);
        if (flag) return true;
    }

    return false;
}

void ray_tracing(const Camera &camera, std::vector<std::reference_wrapper<MeshModel>> mesh_models) {


    auto up = camera.camera_up_axis;
    auto right = camera.camera_right_axis;

    float tmp1 = glm::length(up);
    float tmp2 = glm::length(right);

    auto base = camera.position + camera.camera_front * 0.1f + up * 0.0414f - right * 0.0414f;

    up = up * 0.0414f / float(n / 2);
    right = right * 0.0414f / float(m / 2);

    for (auto &model_ref: mesh_models) {
        auto &model = model_ref.get();
        float minx, miny, minz, maxx, maxy, maxz;
        minx = miny = minz = 1000;
        maxx = maxy = maxz = -1000;
        for (auto &vertex: model.vertices) {
            vertex.point = model.transform * glm::vec4(vertex.point, 1.0f);
            minx = std::min(minx, vertex.point.x);
            miny = std::min(miny, vertex.point.y);
            minz = std::min(minz, vertex.point.z);
            maxx = std::max(maxx, vertex.point.x);
            maxy = std::max(maxy, vertex.point.y);
            maxz = std::max(maxz, vertex.point.z);
        }

        model.set_box(minx, miny, minz, maxx, maxy, maxz);
        std::cout << std::format("{} {} {} {} {} {} \n", minx, miny, minz, maxx, maxy, maxz);
        model.transform = glm::identity<glm::mat4>();
    }

    #pragma omp parallel for num_threads(8)
    for (int i = 0; i < n; i++) {
        std::cerr << i << std::endl;
        for (int j = 0; j < m; j++) {
            auto view_point = base - (up * float(i)) + (right * (float(j)));
            Ray ray(camera.position, view_point - camera.position);

            std::map<float, size_t> param_index_map;

            for (auto k = 0; k < mesh_models.size(); k++) {
                auto &model = mesh_models[k].get();

                if (not ray_tracing_box_test(ray, model.get_box())) {
                    continue;
                }

                for (auto tri: model.faces_indices) {
                    auto [flag, t, u, v, w] = ray.ray_triangle_intersection(
                            model.vertices[tri.x].point,
                            model.vertices[tri.y].point,
                            model.vertices[tri.z].point
                            );
                    if (flag) {
                        param_index_map[t] = k;
                    }
                }
            }

            if (param_index_map.empty()) {
                // std::cout << "no hit" << std::endl;
                image[i][j] = {0, 0, 0};
            } else {
                auto [t, idx] = *param_index_map.begin();
                image[i][j] = mesh_models[idx].get().object_color;
            }
        }
    }

    output_ppm_image(image);
}
#pragma once

#include <vector>
#include <array>
#include <map>

#include "common/ray_tracing/ray.h"
#include "common/camera/camera.hxx"
#include "common/mesh_model.hxx"
#include "common/io/render_output.h"

#include <random>

constexpr const int n = 1024;
constexpr const int m = 1024;

std::array<std::array<glm::vec3, m>, n> image;

struct RGB {
    unsigned char r, g, b;
};

struct RGBA {
    unsigned char r, g, b, a;
};

glm::vec3 get_texture_rgb(Texture texture, float u, float v) {
    int sample_i = int(u * texture.width);
    int sample_j = int(v * texture.height);
    RGB *data = reinterpret_cast<RGB*>(texture.data);
    RGB item = data[sample_j * texture.width + sample_i];
    return { float(item.r) / 255, float(item.g) / 255, float(item.b) / 255 };
}

glm::vec4 get_texture_rgba(Texture texture, float u, float v) {
    int sample_i = int(u * texture.width);
    int sample_j = int(v * texture.height);
    auto *data = reinterpret_cast<RGBA*>(texture.data);
    auto item = data[sample_j * texture.width + sample_i];
    return { float(item.r) / 255, float(item.g) / 255, float(item.b) / 255, float(item.a) / 255 };
}

glm::vec3 get_texture(Texture texture, float u, float v) {
    if (texture.num_channels == 3) {
        return get_texture_rgb(texture, u, v);
    } else {
        return glm::vec3(get_texture_rgba(texture, u, v));
    }
}

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

float shadow_test(glm::vec3 pos, glm::vec3 light_src, std::vector<std::reference_wrapper<MeshModel>> mesh_models) {
    constexpr float eps = 1e-4;

    Ray ray(pos, glm::normalize(light_src - pos));

    float res = 0;

    for (auto k = 0; k < mesh_models.size(); k++) {
        MeshModel &model = mesh_models[k].get();

        if (not ray_tracing_box_test(ray, model.get_box())) {
            continue;
        }

        for (auto &tri: model.faces_indices) {
            auto [flag, t, u, v, w] = ray.ray_triangle_intersection(
                    model.vertices[tri.x].point,
                    model.vertices[tri.y].point,
                    model.vertices[tri.z].point
            );

            if (flag and t > eps) {
                if (model.blending) {
                    auto v0 = model.vertices[tri.x];
                    auto v1 = model.vertices[tri.y];
                    auto v2 = model.vertices[tri.z];

                    auto uv = v0.texture_coord * u + v1.texture_coord * v + v2.texture_coord * w;

                    auto texture_result = get_texture_rgba(model.textures[0], uv.x, uv.y);

                    res = std::max(res, texture_result.w);
                } else {
                    return 1;
                }
            }
        }
    }

    return res;
}

constexpr const int MAX_RAY_TRACING_DEPTH = 3;

glm::vec3 ray_tracing_light(glm::vec3 origin, glm::vec3 direction, int depth, glm::vec3 light_color, std::vector<std::reference_wrapper<MeshModel>> &mesh_models) {
    static glm::vec3 light_src {-7, 7, 10};

    if (depth > MAX_RAY_TRACING_DEPTH) {
        return {0, 0, 0};
    }

    Ray ray(origin, direction);

    std::map<float, std::tuple<size_t, float, float, float, decltype(MeshModel::faces_indices)::value_type>> param_index_map;

    for (auto k = 0; k < mesh_models.size(); k++) {
        auto &model = mesh_models[k].get();

        if (not ray_tracing_box_test(ray, model.get_box())) {
            continue;
        }

        for (auto &tri: model.faces_indices) {
            auto [flag, t, u, v, w] = ray.ray_triangle_intersection(
                    model.vertices[tri.x].point,
                    model.vertices[tri.y].point,
                    model.vertices[tri.z].point
            );
            if (flag and t > 1e-5) {
                param_index_map[t] = {k, u, v, w, tri};
            }
        }
    }

    glm::vec3 object_color {0, 0, 0};

    if (param_index_map.empty()) {
        // std::cout << "no hit" << std::endl;
        return {0, 0, 0};
    } else {
        auto [t, res_tuple] = *param_index_map.begin();
        auto [idx, u, v, w, tri] = res_tuple;

        MeshModel &model = mesh_models[idx].get();
        auto v0 = model.vertices[tri.x];
        auto v1 = model.vertices[tri.y];
        auto v2 = model.vertices[tri.z];

        auto uv = v0.texture_coord * u + v1.texture_coord * v + v2.texture_coord * w;

        bool has_specular_texture = false;
        glm::vec3 specular_texture;

        bool has_blending = false;
        float alpha = 1;

        if (mesh_models[idx].get().textures.empty()) {
            object_color = mesh_models[idx].get().object_color;
        } else {
            glm::vec3 diffuse_texture {0, 0, 0};
            for (auto texture: mesh_models[idx].get().textures) {
                if (texture.type == TextureType::diffuse_texture) {
                    diffuse_texture = get_texture(texture, uv.x, uv.y);
                }
                if (texture.type == TextureType::specular_texture) {
                    specular_texture = get_texture(texture, uv.x, uv.y);
                    has_specular_texture = true;
                }
            }
            object_color = diffuse_texture;

            if (model.blending) {
                auto blending_texture = get_texture_rgba(model.textures[0], uv.x, uv.y);
                alpha = blending_texture.w;

                if (alpha < 0.9) {
                    has_blending = true ;
                }
            }
        }

        constexpr float ambient_strength = 0.2;
        float shadow = 0;

        // compute local ambient
        auto ambient = ambient_strength * light_color;

        // compute local diffuse
        auto normal = v0.normal * u + v1.normal * v + v0.normal * w;
        auto frag_position = ray.at(t);
        auto light_direction = glm::normalize(light_src - frag_position);
        float diffuse_strength = std::max(0.0f, glm::dot(normal, light_direction));
        auto diffuse = diffuse_strength * light_color;

        // compute local specular
        constexpr const float specular_strength = 0.5;
        constexpr const int specular_pow = 32;
        auto view_direction = glm::normalize(origin - frag_position);
        auto reflect_direction = glm::reflect(-light_direction, normal);
        float specular_coefficient = std::pow(std::max(0.0f, glm::dot(view_direction, reflect_direction)), specular_pow);
        auto specular = specular_strength * specular_coefficient * light_color;

        if (has_specular_texture) {
            specular = specular * specular_texture;
        }

        shadow = shadow_test(frag_position, light_src, mesh_models);


        auto local = (ambient + (1.0f - shadow) * (diffuse + specular)) * object_color;

        // compute refraction lighting strength
        if (has_blending) {
            auto refraction = ray_tracing_light(frag_position, direction, depth + 1, light_color, mesh_models);
            local = (1 - shadow) *  object_color * light_color;
            return local * alpha + (1 - alpha) * refraction;
        }

        // compute the mirror reflection
        auto reflect = direction - 2.0f * glm::dot(direction, normal) * normal;
        if (model.reflection) {
            return local +  ray_tracing_light(frag_position, reflect, depth + 1, light_color, mesh_models);
        }

        return local + 0.1f * ray_tracing_light(frag_position, reflect, depth, object_color, mesh_models);
    }
}

void ray_tracing(const Camera &camera, std::vector<std::reference_wrapper<MeshModel>> &mesh_models) {

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

    constexpr int sampling_number_per_pixel = 4;

    #pragma omp parallel for num_threads(8)
    for (int i = 0; i < n; i++) {
        std::cerr << i << std::endl;

        std::mt19937 gen(std::time(nullptr));
        std::uniform_real_distribution<float> delta_i_generator(0, 1);
        std::uniform_real_distribution<float> delta_j_generator(0, 1);
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < sampling_number_per_pixel; k++) {
                auto view_point = base - (up * float(i)) + (right * (float(j)));
                auto delta_i = delta_i_generator(gen);
                auto delta_j = delta_j_generator(gen);
                view_point += (-up * delta_i + right * delta_j);
                Ray ray(camera.position, view_point - camera.position);
                image[i][j] += ray_tracing_light(camera.position, view_point - camera.position, 1, {1, 1, 1}, mesh_models);
            }
            image[i][j] /= float(sampling_number_per_pixel);
        }
    }

    output_ppm_image(image);
}
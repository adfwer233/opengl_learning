#include "common/mesh_model.hxx"

#include "glm/glm.hpp"
#include "common/math/aabb.hxx"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "common/camera/camera.hxx"
#include "stb_image/stb_image.h"
#include <format>

AxisAlignedBoundingBox MeshModel::get_box() const {

    auto dx = box.x_range.end - box.x_range.start;
    auto dy = box.y_range.end - box.y_range.start;
    auto dz = box.z_range.end - box.z_range.start;

    float min_x, min_y, min_z, max_x, max_y, max_z;
    min_x = min_y = min_z = 99999;
    max_x = max_y = max_z = -99999;

    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            for (int k = 0; k <= 1; k++) {
                auto cur_dx = dx * i;
                auto cur_dy = dy * j;
                auto cur_dz = dz * k;

                auto point = glm::vec3(box.x_range.start + cur_dx, box.y_range.start + cur_dy, box.z_range.start + cur_dz);
                auto point_in_world = transform * glm::vec4(point, 1.0);

                min_x = std::min(min_x, point_in_world.x);
                max_x = std::max(max_x, point_in_world.x);
                min_y = std::min(min_y, point_in_world.y);
                max_y = std::max(max_y, point_in_world.y);
                min_z = std::min(min_z, point_in_world.z);
                max_z = std::max(max_z, point_in_world.z);
            }
        }
    }

    // std::cout << min_x << ' ' << max_x << std::endl;

    auto box = AxisAlignedBoundingBox(
        {min_x, min_y, min_z},
        {max_x, max_y, max_z}
    );

    // std::cout << box << std::endl;

    return box;
}

bool MeshModel::collision_test(MeshModel &model1, MeshModel &model2) {
    constexpr float eps = 1e-6;

    auto box1 = model1.get_box();
    auto box2 = model2.get_box();

    // std::cout << box1 << ' ' << box2 << std::endl;

    auto res1 = Interval::intersection_length(box1.x_range, box2.x_range); 
    auto res2 = Interval::intersection_length(box1.y_range, box2.y_range);
    auto res3 = Interval::intersection_length(box1.z_range, box2.z_range);

    // std::cout << std::format("{} {} {}", res1, res2, res3) << std::endl;

    return res1 > -eps and res2 > -eps and res3 > -eps;
}

void MeshModel::bind_buffer() {
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(decltype(vertices)::value_type), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces_indices.size() * sizeof(TriangleVerticeIndex), faces_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices)::value_type), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices)::value_type), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(decltype(vertices)::value_type), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void MeshModel::process_shadow_rendering(Shader& shader) {
    shader.use();
    unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, faces_indices.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void MeshModel::process_rendering(Shader& shader, Camera camera, unsigned int depth_map, glm::vec3 lightPos, glm::vec3 color) {
    auto projection = glm::perspective(glm::radians(camera.zoom), 1.0f * 1024 / 1024, 0.1f, 100.0f);

    shader.use();
    shader.set_vec3("objectColor", color);
    shader.set_vec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.set_vec3("lightPos", lightPos);
    shader.set_vec3("viewPos", camera.position);

    unsigned int transformLoc = glGetUniformLocation(shader.ID, "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(this->transform));

    unsigned int viewTransformLoc = glGetUniformLocation(shader.ID, "view");
    glUniformMatrix4fv(viewTransformLoc, 1, GL_FALSE, glm::value_ptr(camera.get_view_transformation()));

    unsigned int projectionTransformLoc = glGetUniformLocation(shader.ID, "projection");
    glUniformMatrix4fv(projectionTransformLoc, 1, GL_FALSE, glm::value_ptr(projection));

    shader.set_int("use_texture", this->use_texture ? 1 : 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_map);

    if (use_texture) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->texture);
    }

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, faces_indices.size() * 3, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void MeshModel::bind_texture(std::string texture_path) {
    // read the texture image

    int width, height, nrChannels;
    unsigned char *data = stbi_load(texture_path.c_str(), &width, &height, &nrChannels, 0);

    if (data == nullptr) {
        std::cout << "read image failed" << std::endl;
        return;
    }

    this->use_texture = true;

    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    std::cout << "texture id " << this->texture << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);
}
#pragma once

#include <list>
#include <iostream>
#include <vector>

#include "half_edge.hxx"
#include "point.hxx"
#include "common/math/aabb.hxx"
#include "shader.hxx"
#include "common/camera/camera.hxx"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum TextureType {
    diffuse_texture,
    specular_texture,
    normal_texture,
    height_texture
};

struct Texture {
    unsigned int id;
    TextureType type;
    std::string path;
    unsigned char* data;
    int num_channels;
    int width, height;
};

struct TriangleVerticeIndex {
	unsigned int x, y, z;
};

struct TriangleWithNormal {
	glm::vec3 point, normal;
    glm::vec2 texture_coord;
};

struct MeshModel {

private:
	AxisAlignedBoundingBox box;

public:
	std::vector<TriangleWithNormal> vertices;
	std::vector<TriangleVerticeIndex> faces_indices;

	glm::mat4 transform;

    bool blending{false};

    glm::vec3 object_color{1, 1, 1};

    unsigned int VBO{}, VAO{}, EBO{};

    std::vector<Texture> textures;

	MeshModel() : transform(glm::mat4(1.0f)), box(AxisAlignedBoundingBox({ 0, 0, 0 }, { 0, 0, 0 })) {}

	AxisAlignedBoundingBox get_box() const;

    void bind_buffer();

    void bind_texture(const std::string& texture_path , TextureType type);

    void process_shadow_rendering(Shader& shader);

    void process_rendering(Shader& shader, Camera camera, unsigned int depth_map, glm::vec3 lightPos);

	static bool collision_test(MeshModel &model1, MeshModel &model2);

	friend class Constructor;

    void bind_texture_with_alpha(const std::string& texture_path, TextureType type);

    float get_distance(glm::vec3 pos) const;

    void process_environment_reflection_rendering(Shader &shader, Camera camera, unsigned int skybox_texture);

    void set_box(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);
};
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



struct TriangleVerticeIndex {
	unsigned int x, y, z;
};

struct TriangleWithNormal {
	Point3d point, normal;
    Point2d texture_coord;
};

struct MeshModel {

private:
	AxisAlignedBoundingBox box;

public:
	std::vector<TriangleWithNormal> vertices;
	std::vector<TriangleVerticeIndex> faces_indices;

	glm::mat4 transform;

    unsigned int VBO, VAO, EBO;

    unsigned int texture;

    bool use_texture;

	MeshModel() : transform(glm::mat4(1.0f)), box(AxisAlignedBoundingBox({ 0, 0, 0 }, { 0, 0, 0 })), use_texture(false) {}

	AxisAlignedBoundingBox get_box() const;

    void bind_buffer();

    void bind_texture(std::string texture_path);

    void process_shadow_rendering(Shader& shader);

    void process_rendering(Shader& shader, Camera camera, unsigned int depth_map, glm::vec3 lightPos, glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f));

	static bool collision_test(MeshModel &model1, MeshModel &model2);

	friend class Constructor;

    void bind_texture_with_alpha(std::string texture_path);

    float get_distance(glm::vec3 pos) const;
};
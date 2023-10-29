#pragma once

#include "common/mesh_model.hxx"
#include "common/math/vector_field.hxx"
#include <optional>

class SolidEntity {

public:
	std::optional<std::reference_wrapper<MeshModel>> model;

	// the mess center in the model space
	glm::vec3 mass_center;

	float mass;

	glm::vec3 velocity;

	SolidEntity() : mass(1), mass_center(glm::vec3(0)), velocity(glm::vec3(0, 0, 0)) {}

	glm::vec3 get_world_mass_center() const;

	void update_in_vector_field(VectorField field, float delta_time);
};
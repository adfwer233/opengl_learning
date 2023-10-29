#include "common/simulation/solid_entity.hxx"

#include <format>

glm::vec3 SolidEntity::get_world_mass_center() const {
	if (not model.has_value())
		return glm::vec3(0.0f);

	return glm::vec3(model.value().get().transform * glm::vec4(mass_center, 1));
}

void SolidEntity::update_in_vector_field(VectorField field, float delta_time) {
	if (mass == 0) return;

	delta_time /= 1;
	for (int i = 0; i < 1; i++) {
		// ex-euler method
		auto world_center = get_world_mass_center();

		auto force = field.eval(world_center);

		auto next_pos = world_center + glm::normalize(velocity) * 0.0005f;

		auto force_pos = field.eval(world_center);
		auto force_next_pos = field.eval(next_pos);

		force = (force_pos + force_next_pos) * 0.5f;

		auto acceleration = force / mass;

		velocity = velocity + acceleration * delta_time;

		model.value().get().transform = glm::translate(model.value().get().transform, velocity * delta_time);
	}
}
#include "common/simulation/solid_entity.hxx"

glm::vec3 SolidEntity::get_world_mass_center() const {
    if (not model.has_value())
        return glm::vec3(0.0f);
    
    return glm::vec3(model.value().get().transform * glm::vec4(mass_center, 1));
}

void SolidEntity::update_in_vector_field(VectorField field, float delta_time) {
    if (mass == 0) return;

    auto world_center = get_world_mass_center();

    auto force = field.eval(world_center);

    delta_time /= 100;
    for (int i = 0; i < 100; i++) {
        velocity = velocity + force * delta_time / mass;

        auto acceleration = force / mass;

        std::cout << delta_time << std::endl;

        model.value().get().transform = glm::translate(model.value().get().transform, velocity * delta_time + glm::vec3(0.5) * acceleration * delta_time * delta_time);
    }
}
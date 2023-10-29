#include "common/math/vector_field.hxx"

glm::vec3 VectorField::eval(glm::vec3 pos) const {
    return field(pos);
}
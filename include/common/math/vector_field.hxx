#pragma once

#include <functional>

#include "glm/glm.hpp"

class VectorField {

private:
    std::function<glm::vec3(glm::vec3)> field;

public:
    VectorField(std::function<glm::vec3(glm::vec3)> t_field): field(t_field) {};

    glm::vec3 eval(glm::vec3) const;
};
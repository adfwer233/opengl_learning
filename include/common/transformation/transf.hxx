#pragma once

#include "glm/matrix.hpp"

#include "../point.hxx"

struct transf {
    glm::mat3x3 transf_matrix;

    transf operator * (transf t_transf) const;
};

transf translate_transf(Point3d vec);
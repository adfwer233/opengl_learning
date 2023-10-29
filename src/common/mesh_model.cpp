#include "common/mesh_model.hxx"

#include "glm/glm.hpp"

AxisAlignedBoundingBox MeshModel::get_box() const {
    auto min_pt = transform * glm::vec4(box.x_range.start, box.y_range.start, box.z_range.start, 1.0);
    auto max_pt = transform * glm::vec4(box.x_range.end, box.y_range.end, box.z_range.end, 1.0);
    return AxisAlignedBoundingBox(
        {min_pt.x, min_pt.y, min_pt.z},
        {max_pt.x, max_pt.y, max_pt.z}
    );
}
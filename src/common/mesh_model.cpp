#include "common/mesh_model.hxx"

#include "glm/glm.hpp"
#include "common/math/aabb.hxx"
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
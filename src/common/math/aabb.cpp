#include "common/math/aabb.hxx"

std::ostream& operator << (std::ostream& os, AxisAlignedBoundingBox box)  {
    os << std::format("({} {}) ({} {}) ({} {})", box.x_range.start, box.x_range.end, box.y_range.start, box.y_range.end, box.z_range.start, box.z_range.end);
    return os;
}
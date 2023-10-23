#pragma once

#include "interval.hxx"

struct AxisAlignedBoundingBox {
    Interval x_range;
    Interval y_range;
    Interval z_range;

    static bool is_aabb_overlap(AxisAlignedBoundingBox box1, AxisAlignedBoundingBox box2);
};
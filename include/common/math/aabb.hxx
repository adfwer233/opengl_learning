#pragma once

#include "interval.hxx"

struct AxisAlignedBoundingBox {
    Interval x_range;
    Interval y_range;
    Interval z_range;

    AxisAlignedBoundingBox(Point3d min_pt, Point3d max_pt) {
        x_range = Interval(min_pt.x, max_pt.x);
        y_range = Interval(min_pt.y, max_pt.y);
        z_range = Interval(min_pt.z, max_pt.z);
    }

    static bool is_aabb_overlap(AxisAlignedBoundingBox box1, AxisAlignedBoundingBox box2);
};
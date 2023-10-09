#pragma once

#include<tuple>
#include "polygon.hxx"

constexpr float epsilon = 1e-6;
using point = std::tuple<float, float>;

// Todo: use concept and template
float outer_product(float x1, float y1, float x2, float y2);

std::tuple<bool, float, float> line_segment_intersector(point a, point b, point c, point d);

std::vector<point> polygon_intersect_points(Polygon& poly1, Polygon& poly2);
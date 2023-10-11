#pragma once

#include<tuple>
#include "polygon.hxx"
#include "point.hxx"

constexpr float epsilon = 1e-6;


template<typename T>
concept Floating = std::is_floating_point<T>::value;

template<typename T>
T outer_product(T x1, T y1, T x2, T y2) requires Floating<T> {
	return x1 * y2 - x2 * y1;
}

std::tuple<bool, float, float> line_segment_intersector(Point2d a, Point2d b, Point2d c, Point2d d);

std::vector<Point2d> polygon_intersect_points(Polygon& poly1, Polygon& poly2);

/*
    Compute the intersection between a line segment and a polygon
    Input: line segment from $a$ to $b$, and a polygon $poly$
    Output: a vector of poins, including $a$, $b$ and intersecting points, order by distance from $a$
            a bool is used to represent whether this point is "into point"
*/
std::vector<std::tuple<Point2d, bool>> intersect_segment_polygon(Point2d a, Point2d b, Polygon& poly);

bool is_loop_polygon_intersected(half_edge_loop loop, Polygon &poly);
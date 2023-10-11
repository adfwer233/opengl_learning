#pragma once

#include "point.hxx"
#include "polygon.hxx"

double vector_angle(Point2d p1, Point2d p2);

/*
    judge whether a point is in a polygon by winding number
*/
bool point_in_polygon(Point2d point, Polygon &poly);
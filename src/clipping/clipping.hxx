#pragma once

#include <algorithm>
#include <map>
#include <set>

#include "common/intersector.hxx"
#include "common/polygon.hxx"

class Weiler_Atherton {

private:
	enum point_type {
		normal, in, out
	};

	half_edge_edge* break_edge_by_vertex(half_edge_edge* edge, half_edge_vertex* vertex) {
		auto new_edge = new half_edge_edge();
		auto new_edge_twin = new half_edge_edge();

		new_edge->succ = edge->succ;
		new_edge->face = edge->face;
		new_edge->twin = new_edge_twin;
		new_edge->vertex = vertex;

		new_edge_twin->succ = edge->twin->succ;
		new_edge_twin->face = edge->twin->face;
		new_edge_twin->twin = new_edge;
		new_edge_twin->vertex = edge->twin->vertex;

		edge->succ = new_edge;
		edge->twin->vertex = vertex;

		vertex->face = edge->face;
		vertex->edge = new_edge;
		return new_edge;
	}

public:
	/*
		Weiler-Atherton Algorithm: Window clipping for arbitrary polygons.

		Input: two polygon represented in half-edge data structure
		Output: Clipped polygon

		Alogrithm:
			Step1: Finding all intersect points of both polygons, judge type (in or out) and create vertex array
				For all edges in polygon and window, split the edges by intersect points
				save the double-sided pointer in a map<vertex*, vertex*>

			Step2: Traverse the array and construct the loops
			Step3: construct the half-edge polygon from loops
	*/
	Polygon weiler_atherton_algorithm(Polygon& poly, Polygon& window);

};
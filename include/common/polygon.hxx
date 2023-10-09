#pragma once

#include<vector>
#include<iostream>
#include<format>
#include<ranges>
#include "half_edge.hxx"

struct Polygon {

	half_edge_face* polygon = nullptr;

	~Polygon() {
		delete polygon;
	}

	Polygon() {
		polygon = new half_edge_face();
	}

    void print_polygon_message();

	void add_loop(std::vector<std::tuple<float, float, float>> vertices, bool is_inner = false);

};
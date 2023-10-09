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

	void print_polygon_message() {
		std::cout << std::format("{:=^50}\n", "polygon message");
		std::cout << std::format("vertex count {}, loops count {}, by euler formula, edge count {}\n", polygon->vertices.size(), polygon->loops.size(), polygon->vertices.size() + polygon->loops.size() - 2);
		std::cout << std::format("{:=^50}\n", "loop messages");

		int i = 0;
		for (auto loop : polygon->loops) {
			std::string msg = std::format("loop {}", i);
			std::cout << std::format("{:=^30}\n", msg);
			auto start_edge = loop.start;
			auto current_edge = start_edge;

			while (true) {
				auto twin = current_edge->twin;
				std::cout << std::format("edge from ({}, {}) to ({}, {})\n", current_edge->vertex->x, current_edge->vertex->y, twin->vertex->x, twin->vertex->y);
				current_edge = current_edge->succ;
				if (current_edge == start_edge) break;
			}
		}
	}

	void add_loop(std::vector<std::tuple<float, float, float>> vertices, bool is_inner = false) {
		std::list<half_edge_vertex*> vertices_list;
		std::list<half_edge_edge*> edges_list;

		auto [x, y, z] = vertices[0];
		auto first_vertex = new half_edge_vertex{ x, y, z };
		vertices_list.push_back(first_vertex);


		for (int i = 1; i < vertices.size(); i++) {
			auto edge = new half_edge_edge();
			auto edge_twin = new half_edge_edge();

			auto [x, y, z] = vertices[i];
			auto new_vertex = new half_edge_vertex{ x, y, z };

			edge->vertex = vertices_list.front();
			edge_twin->vertex = new_vertex;

			edge->face = is_inner ? nullptr : polygon;
			edge_twin->face = is_inner ? polygon : nullptr;

			edge->twin = edge_twin;
			edge_twin->twin = edge;

			if (not edges_list.empty()) {
				edges_list.back()->succ = edge;
				edge_twin->succ = edges_list.back()->twin;
			}

			vertices_list.back()->edge = edge;
			vertices_list.back()->face = polygon;

			edges_list.push_back(edge);
			vertices_list.push_back(new_vertex);

		}

		vertices_list.back()->edge = edges_list.front();
		vertices_list.back()->face = polygon;

		edges_list.back()->succ = edges_list.front();
		edges_list.front()->twin->succ = edges_list.back()->twin;

		polygon->vertices.splice(polygon->vertices.end(), vertices_list);
		polygon->loops.push_back(half_edge_loop{ edges_list.front() });
	}
};
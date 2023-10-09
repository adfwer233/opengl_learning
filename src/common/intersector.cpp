#include <algorithm>

#include <concepts>

#include "common/intersector.hxx"

std::tuple<bool, float, float> line_segment_intersector(Point2d a, Point2d b, Point2d c, Point2d d) {

	auto [ax, ay] = a;
	auto [bx, by] = b;
	auto [cx, cy] = c;
	auto [dx, dy] = d;

	float area_abc = outer_product(ax - cx, ay - cy, bx - cx, by - cy);
	float area_abd = outer_product(ax - dx, ay - dy, bx - dx, by - dy);
	float area_cda = outer_product(cx - ax, cy - ay, dx - ax, dy - ay);
	float area_cdb = outer_product(cx - bx, cy - by, dx - bx, dy - by);

	if (area_abc * area_abd > -epsilon or area_cda * area_cdb > -epsilon)
		return std::make_tuple(false, 0, 0);

	float t = area_cda / (area_abd - area_abc);
	float dx1 = t * (bx - ax);
	float dy1 = t * (by - ay);

	return std::make_tuple(true, ax + dx1, ay + dy1);
}

std::vector<Point2d> polygon_intersect_points(Polygon& poly1, Polygon& poly2) {
	std::vector<Point2d> result;

	for (auto poly1_loop : poly1.polygon->loops) {
		auto loop1_start = poly1_loop.start;
		auto cur_edge1 = loop1_start;

		while (true) {

			for (auto poly2_loop : poly2.polygon->loops) {
				auto loop2_start = poly2_loop.start;
				auto cur_edge2 = loop2_start;

				while (true) {
					auto [flag, x, y] = line_segment_intersector(
						{cur_edge1->vertex->x, cur_edge1->vertex->y},
						{cur_edge1->twin->vertex->x, cur_edge1->twin->vertex->y},
						{cur_edge2->vertex->x, cur_edge2->vertex->y},
						{cur_edge2->twin->vertex->x, cur_edge2->twin->vertex->y}
					);

					if (flag)
						result.push_back(Point2d{x, y});
					
					cur_edge2 = cur_edge2->succ;
					if (cur_edge2 == loop2_start) break;
				}
			}

			cur_edge1 = cur_edge1->succ;
			if (cur_edge1 == loop1_start) break;
		}
	}

	std::cout << result.size() << std::endl;
	return result;
}

std::vector<std::tuple<Point2d, bool>> intersect_segment_polygon(Point2d a, Point2d b, Polygon &poly) {
	std::vector<std::tuple<Point2d, bool>> result;

	auto edges = poly.get_all_edges();

	auto [ax, ay] = a;
	auto [bx, by] = b;
	
	for (auto edge: edges) {
		auto [flag, x, y] = line_segment_intersector(
			a, b, {edge->vertex->x, edge->vertex->y}, {edge->twin->vertex->x, edge->twin->vertex->y}
		);

		if (flag) {
			auto tmp = outer_product(edge->vertex->x - ax, edge->vertex->y - ay, bx - ax, by - ay);
			result.push_back(std::make_tuple(Point2d{x, y}, tmp > 0));
		}
	}

	std::ranges::sort(result, [&](auto t1, auto t2) -> bool {
		Point2d p1 = std::get<0>(t1);
		Point2d p2 = std::get<0>(t2);
		return abs(p1.x - ax) < abs(p2.x - ax);
	});

	return result;
}
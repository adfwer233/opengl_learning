#include "clipping.hxx"

Polygon Weiler_Atherton::weiler_atherton_algorithm(Polygon &poly, Polygon &window) {
    std::map<half_edge_vertex*, half_edge_vertex*> partner_map;
    std::map<half_edge_vertex*, point_type> point_type_map;
    std::set<half_edge_vertex*> inter_point_set;

    // Step 1:
    for (auto poly_loop: poly.polygon->loops) {
        auto poly_loop_start = poly_loop.start;
        auto poly_loop_current = poly_loop_start;

        while(true) {
            
            std::vector<half_edge_vertex*> new_vertices;

            for (auto window_loop: window.polygon->loops) {
                auto window_loop_start = window_loop.start;
                auto window_loop_current = window_loop_start;

                while (true) {
                    auto [flag, x, y] = line_segment_intersector(
                        std::make_tuple(poly_loop_current->vertex->x, poly_loop_current->vertex->y),
                        std::make_tuple(poly_loop_current->twin->vertex->x, poly_loop_current->twin->vertex->y),
                        std::make_tuple(window_loop_current->vertex->x, window_loop_current->vertex->y),
                        std::make_tuple(window_loop_current->twin->vertex->x, window_loop_current->twin->vertex->y)
                    );

                    std::cout << std::format("inter {} {}\n", x, y);

                    if (flag) {
                        auto vertex = new half_edge_vertex{x, y, 0};
                        auto vertex_partner = new half_edge_vertex{x, y, 0};

                        // get the type
                        auto res = outer_product(
                            poly_loop_current->vertex->x - window_loop_current->vertex->x,
                            poly_loop_current->vertex->y - window_loop_current->vertex->x,
                            window_loop_current->twin->vertex->x - window_loop_current->vertex->x,
                            window_loop_current->twin->vertex->y - window_loop_current->vertex->y
                        );

                        point_type_map[vertex] = res > 0 ? in : out;
                        point_type_map[vertex_partner] = res > 0 ? in : out;

                        partner_map[vertex] = vertex_partner;
                        partner_map[vertex_partner] = vertex;

                        new_vertices.push_back(vertex_partner);
                        inter_point_set.insert(vertex_partner);

                        // break the edge in window
                        window_loop_current = break_edge_by_vertex(window_loop_current, vertex);
                    }

                    window_loop_current = window_loop_current->succ;
                    if(window_loop_current == window_loop_start) break;
                }
            }

            // break the edge in polygon by intersect vertices

            std::ranges::sort(new_vertices, [&](auto a, auto b) -> bool {
                return abs(a->x - poly_loop_current->vertex->x) < abs(b->x - poly_loop_current->vertex->x);
            });

            for (auto v: new_vertices) {
                poly_loop_current = break_edge_by_vertex(poly_loop_current, v);
            }

            poly_loop_current = poly_loop_current->succ;
            if (poly_loop_current == poly_loop_start) break;
        }
    }
    
    Polygon result;

    // Step 2:
    while (not inter_point_set.empty()) {
        auto start = *inter_point_set.begin();
        auto current = start;

        std::vector<std::tuple<float, float, float>> result_vertices;

        while (true) {

            std::cout << std::format("traversing {} {}\n", current->x, current->y);
            
            if (point_type_map[current] != normal) {
                if (current->face == poly.polygon)
                    inter_point_set.erase(current);
                else
                    inter_point_set.erase(partner_map[current]);
            }
            
            result_vertices.push_back(std::make_tuple(current->x, current->y, 0));

            if (point_type_map[current] == normal) {
                /*
                    case1: normal
                    just move to next vertex
                */
                current = current->edge->twin->vertex;
            } else if (point_type_map[current] == in) {
                /*
                    case2: in point
                    switch to partner if in window now
                */
                if (current->face == poly.polygon) {
                    current = current->edge->twin->vertex;
                } else {
                    current = partner_map[current]->edge->twin->vertex;
                }
            } else if (point_type_map[current] == out) {
                /*
                    case3: out point
                    switch to partner if in polygon now
                */
                if (current->face == poly.polygon) {
                    current = partner_map[current]->edge->twin->vertex;
                } else {
                    current = current->edge->twin->vertex;
                }
            }
            
            if (current == start or current == partner_map[start]) break;

        }

        result.add_loop(result_vertices);
    }

    return result;
}
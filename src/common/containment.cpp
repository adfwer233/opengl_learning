#include "common/containment.hxx"

double vector_angle(Point2d p1, Point2d p2) {
    p1 = normalize(p1);
    p2 = normalize(p2);
    double outer = p1.x * p2.y - p1.y * p2.x;
    double inner = inner_product(p1, p2);
    double angle = acos(inner);
    std::cout << "inner " << inner << std::endl;
    return outer > 0 ? angle : -angle;
}


bool point_in_polygon(Point2d point, Polygon &poly) {
    double winding_number = 0;

    for (auto loop: poly.polygon->loops) {
        auto start = loop.start;
        auto current = start;

        while (true) {

            Point2d p1 { current->vertex->x - point.x, current->vertex->y - point.y };
            Point2d p2 { current->twin->vertex->x - point.x, current->twin->vertex->y - point.y };

            winding_number += vector_angle(p1, p2);

            current = current->succ;
            if (current == start) break;
        }
    }

    std::cout << "winding number " << winding_number << std::endl;

    return winding_number > 3.14;
}
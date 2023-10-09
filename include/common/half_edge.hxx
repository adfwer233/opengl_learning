#include<list>

struct half_edge_vertex {
    float x, y ,z;
    half_edge_face *face;
    half_edge_edge *edge;
};

struct half_edge_edge {
    half_edge_vertex *vertex;
    half_edge_edge *twin;
    half_edge_edge *succ;
    half_edge_face *face;
};

struct half_edge_face {
    std::list<half_edge_vertex> vertices;
    half_edge_edge *edge;
};
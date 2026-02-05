#include "crystalModel.h"

Model crystal_gen(Vertex vert[6], Polygon poly[8], f32 span){
    vert[0] = vertex_new(0.f, -0.5f, 0.f);
    vert[1] = vertex_new(0.f, 0.5f, 0.f);
    vert[2] = vertex_new(-span, 0.f, -span);
    vert[3] = vertex_new(span, 0.f, -span);
    vert[4] = vertex_new(-span, 0.f, span);
    vert[5] = vertex_new(span, 0.f, span);
    poly[0] = polygon_new(0, 2, 3, 0);
    poly[1] = polygon_new(0, 3, 5, 0);
    poly[2] = polygon_new(0, 5, 4, 0);
    poly[3] = polygon_new(0, 4, 2, 0);
    poly[4] = polygon_new(1, 2, 3, 0);
    poly[5] = polygon_new(1, 3, 5, 0);
    poly[6] = polygon_new(1, 5, 4, 0);
    poly[7] = polygon_new(1, 4, 2, 0);
    return model(vert, 6, poly, 8);
}
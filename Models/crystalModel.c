#include "crystalModel.h"

Model crystal_gen(Vertex vert[6], Polygon poly[8], f32 span){
    vert[0] = vertex(0.f, -0.5f, 0.f);
    vert[1] = vertex(0.f, 0.5f, 0.f);
    vert[2] = vertex(-span, 0.f, -span);
    vert[3] = vertex(span, 0.f, -span);
    vert[4] = vertex(-span, 0.f, span);
    vert[5] = vertex(span, 0.f, span);
    poly[0] = polygon(0, 2, 3, 0);
    poly[1] = polygon(0, 3, 5, 0);
    poly[2] = polygon(0, 5, 4, 0);
    poly[3] = polygon(0, 4, 2, 0);
    poly[4] = polygon(1, 2, 3, 0);
    poly[5] = polygon(1, 3, 5, 0);
    poly[6] = polygon(1, 5, 4, 0);
    poly[7] = polygon(1, 4, 2, 0);
    return model(vert, 6, poly, 8);
}
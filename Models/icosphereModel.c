#include "icosphereModel.h"

Model icosphere_gen(Vertex vert[12], Polygon poly[20], unsigned subDiv){
    unsigned curVert = 12, curPol = 20;
    const float w = 0.025f * sqrtf(5.f + sqrtf(5.f)) * (5.f * sqrtf(2.f) - sqrtf(10.f));
    const float h = w * (1.f + sqrtf(5)) * 0.5f;
    float s, c;
    sincosf(SDL_PI_F / 2.f, &s, &c);
    vert[0] = vertex(-w, -h, 0),
    vert[1] = vertex(-w, h, 0),
    vert[2] = vertex(w, h, 0),
    vert[3] = vertex(w, -h, 0);
    for(int i = 0; i < 4; ++i){
        vert[i + 4] = vertex_rotX(vert + i, s, c);
        vert[i + 4] = vertex_rotZ(vert + i + 4, s, c);
        vert[i + 8] = vertex_rotZ(vert + i, s, c);
        vert[i + 8] = vertex_rotX(vert + i + 8, s, c);
    }
    Uint16 t1 = 0, t2 = 4, t3 = 8;
    poly[0]  = polygon(t1+0, t1+3, t2+0, 0);
    poly[1]  = polygon(t1+0, t1+3, t2+1, 0);
    poly[2]  = polygon(t1+0, t2+0, t3+1, 0);
    poly[3]  = polygon(t1+0, t3+1, t3+2, 0);
    poly[4]  = polygon(t1+0, t2+1, t3+2, 0);
    poly[5]  = polygon(t1+1, t2+3, t3+1, 0);
    poly[6]  = polygon(t2+0, t2+3, t3+1, 0);
    poly[7]  = polygon(t1+1, t3+1, t3+2, 0);
    poly[8]  = polygon(t1+1, t2+2, t3+2, 0);
    poly[9]  = polygon(t2+1, t2+2, t3+2, 0);
    poly[10] = polygon(t1+1, t1+2, t2+3, 0);
    poly[11] = polygon(t1+1, t1+2, t2+2, 0);
    poly[12] = polygon(t2+0, t2+3, t3+0, 0);
    poly[13] = polygon(t1+2, t2+3, t3+0, 0);
    poly[14] = polygon(t1+3, t2+0, t3+0, 0);
    poly[15] = polygon(t1+2, t3+0, t3+3, 0);
    poly[16] = polygon(t1+2, t2+2, t3+3, 0);
    poly[17] = polygon(t1+3, t3+0, t3+3, 0);
    poly[18] = polygon(t1+3, t2+1, t3+3, 0);
    poly[19] = polygon(t2+1, t2+2, t3+3, 0);
    return model(vert, curVert, poly, curPol);
}
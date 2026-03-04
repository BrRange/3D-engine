#include "icosphereModel.h"

Model icosphere_gen(Vertex vert[12], Polygon poly[20], unsigned subDiv){
    (void) subDiv;
    unsigned curVert = 12, curPol = 20;
    const f32 w = 0.025f * sqrtf(5.f + sqrtf(5.f)) * (5.f * sqrtf(2.f) - sqrtf(10.f));
    const f32 h = w * (1.f + sqrtf(5)) * 0.5f;
    vert[0] = vertex_new(-w, -h, 0),
    vert[1] = vertex_new(-w, h, 0),
    vert[2] = vertex_new(w, h, 0),
    vert[3] = vertex_new(w, -h, 0);
    Quaternion
    rotX = quat_new(SDL_PI_F / 2.f, vertex_new(1, 0, 0)),
    rotZ = quat_new(SDL_PI_F / 2.f, vertex_new(0, 0, 1));
    for(int i = 0; i < 4; ++i){
        vert[i + 4] = vertex_rotate(vert[i], rotX);
        vert[i + 4] = vertex_rotate(vert[i + 4], rotZ);
        vert[i + 8] = vertex_rotate(vert[i], rotZ);
        vert[i + 8] = vertex_rotate(vert[i + 8], rotX);
    }
    u16 t1 = 0, t2 = 4, t3 = 8;
    poly[0]  = polygon_new(t1+0, t1+3, t2+0, 0);
    poly[1]  = polygon_new(t1+0, t1+3, t2+1, 0);
    poly[2]  = polygon_new(t1+0, t2+0, t3+1, 0);
    poly[3]  = polygon_new(t1+0, t3+1, t3+2, 0);
    poly[4]  = polygon_new(t1+0, t2+1, t3+2, 0);
    poly[5]  = polygon_new(t1+1, t2+3, t3+1, 0);
    poly[6]  = polygon_new(t2+0, t2+3, t3+1, 0);
    poly[7]  = polygon_new(t1+1, t3+1, t3+2, 0);
    poly[8]  = polygon_new(t1+1, t2+2, t3+2, 0);
    poly[9]  = polygon_new(t2+1, t2+2, t3+2, 0);
    poly[10] = polygon_new(t1+1, t1+2, t2+3, 0);
    poly[11] = polygon_new(t1+1, t1+2, t2+2, 0);
    poly[12] = polygon_new(t2+0, t2+3, t3+0, 0);
    poly[13] = polygon_new(t1+2, t2+3, t3+0, 0);
    poly[14] = polygon_new(t1+3, t2+0, t3+0, 0);
    poly[15] = polygon_new(t1+2, t3+0, t3+3, 0);
    poly[16] = polygon_new(t1+2, t2+2, t3+3, 0);
    poly[17] = polygon_new(t1+3, t3+0, t3+3, 0);
    poly[18] = polygon_new(t1+3, t2+1, t3+3, 0);
    poly[19] = polygon_new(t2+1, t2+2, t3+3, 0);
    return model(vert, curVert, poly, curPol);
}
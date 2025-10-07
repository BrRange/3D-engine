#include "icosphere.h"

/**
 * \param vert [12], doubles in size for each subDivision
 * \param pol [20], quadruples in size for each subDivision
 */
Model icosphere_gen(Vertex vert[12], Poly pol[20], unsigned subDiv){
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
    pol[0]  = poly(t1+0, t1+3, t2+0, 0);
    pol[1]  = poly(t1+0, t1+3, t2+1, 0);
    pol[2]  = poly(t1+0, t2+0, t3+1, 0);
    pol[3]  = poly(t1+0, t3+1, t3+2, 0);
    pol[4]  = poly(t1+0, t2+1, t3+2, 0);
    pol[5]  = poly(t1+1, t2+3, t3+1, 0);
    pol[6]  = poly(t2+0, t2+3, t3+1, 0);
    pol[7]  = poly(t1+1, t3+1, t3+2, 0);
    pol[8]  = poly(t1+1, t2+2, t3+2, 0);
    pol[9]  = poly(t2+1, t2+2, t3+2, 0);
    pol[10] = poly(t1+1, t1+2, t2+3, 0);
    pol[11] = poly(t1+1, t1+2, t2+2, 0);
    pol[12] = poly(t2+0, t2+3, t3+0, 0);
    pol[13] = poly(t1+2, t2+3, t3+0, 0);
    pol[14] = poly(t1+3, t2+0, t3+0, 0);
    pol[15] = poly(t1+2, t3+0, t3+3, 0);
    pol[16] = poly(t1+2, t2+2, t3+3, 0);
    pol[17] = poly(t1+3, t3+0, t3+3, 0);
    pol[18] = poly(t1+3, t2+1, t3+3, 0);
    pol[19] = poly(t2+1, t2+2, t3+3, 0);
    return model(vert, curVert, pol, curPol);
}
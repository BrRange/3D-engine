#ifndef CRYSTAL_MODEL_H
#define CRYSTAL_MODEL_H

#include "../fold.h"

/**
 * \param vert [6]
 * \param poly [8]
 */
Model crystal_gen(Vertex vert[6], Polygon poly[8], f32 span);

#endif
#include "../fold.h"

#ifndef CRYSTAL_MODEL_H
#define CRYSTAL_MODEL_H

/**
 * \param vert [6]
 * \param poly [8]
 */
Model crystal_gen(Vertex vert[6], Polygon poly[8], float span);

#endif
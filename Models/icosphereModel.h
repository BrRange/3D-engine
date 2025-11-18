#ifndef ICOSPHERE_MODEL_H_
#define ICOSPHERE_MODEL_H_

#include "../fold.h"

/**
 * \param vert [12], doubles in size for each subDivision
 * \param poly [20], quadruples in size for each subDivision
 */
Model icosphere_gen(Vertex vert[12], Polygon poly[20], unsigned subDiv);

#endif
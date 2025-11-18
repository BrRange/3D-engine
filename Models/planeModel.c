#include "planeModel.h"

Model plane_gen(Vertex vert[4], Polygon poly[2]){
  vert[0] = vertex(0.5f, 0, 0.5f);
  vert[1] = vertex(-0.5f, 0, 0.5f);
  vert[2] = vertex(-0.5f, 0, -0.5f);
  vert[3] = vertex(0.5f, 0, -0.5f);
  poly[0] = polygon(0, 1, 3, 0);
  poly[1] = polygon(1, 2, 3, 0);
  return model(vert, 4, poly, 2);
}
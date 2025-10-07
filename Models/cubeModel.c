#include "cubeModel.h"

Model cube_gen(Vertex vert[8], Polygon poly[12]){
  Uint32 i;
  for(i = 0; i < 8; ++i)
    vert[i].x = i & 0b1 ? -0.5f : 0.5f,
    vert[i].y = i & 0b10 ? -0.5f : 0.5f,
    vert[i].z = i & 0b100 ? -0.5f : 0.5f;
  for(i = 0; i < 2; ++i)
    poly[i] = polygon(i, 1 + i, 2 + i, 0),
    poly[2 + i] = polygon(4 + i, 5 + i, 6 + i, 0),
    poly[4 + i] = polygon(i, 2 + i, 4 + i, 0);
  for(; i < 4; ++i)
    poly[4 + i] = polygon(i, 2 + i, 4 + i, 0);
  poly[8] = polygon(0, 1, 4, 0);
  poly[9] = polygon(1, 4, 5, 0);
  poly[10] = polygon(2, 3, 6, 0);
  poly[11] = polygon(3, 6, 7, 0);
  return model(vert, 8, poly, 12);
}
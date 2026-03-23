#include "planeModel.h"

Model plane_gen(Vec3 vert[4], Polygon poly[2]){
  vert[0] = vec3_new(0.5f, 0, 0.5f);
  vert[1] = vec3_new(-0.5f, 0, 0.5f);
  vert[2] = vec3_new(-0.5f, 0, -0.5f);
  vert[3] = vec3_new(0.5f, 0, -0.5f);
  poly[0] = polygon_new(0, 1, 3, 0);
  poly[1] = polygon_new(1, 2, 3, 0);
  return model(vert, 4, poly, 2);
}
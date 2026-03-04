#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "vertex.h"

typedef struct Quaternion{
  float r, x, y, z;
} Quaternion;

Quaternion quat_new(float angle, Vertex normAxis);

Quaternion quat_compose(Quaternion quat, Quaternion val);

Quaternion quat_conjugate(Quaternion quat);

Quaternion vertex_quat(Vertex point);

Vertex quat_vertex(Quaternion quat);

Vertex vertex_rotate(Vertex point, Quaternion quat);

#endif
#ifndef VERTEX_H_
#define VERTEX_H_

#include "rustydef.h"

typedef struct Vertex{
  f32 x, y, z;
} Vertex;

Vertex vertex_new(f32 x, f32 y, f32 z);

Vertex vertex_getClip(Vertex clip, Vertex unclip, f32 z);

int vertex_projectionCompare(Vertex *a, Vertex *b);

/* Vertex operators */

Vertex vertex_add(Vertex a, Vertex b);

Vertex vertex_sub(Vertex a, Vertex b);

Vertex vertex_scalarMul(Vertex v, f32 scalar);

Vertex vertex_scalarDiv(Vertex v, f32 scalar);

f32 vertex_dot(Vertex a, Vertex b);

Vertex vertex_cross(Vertex a, Vertex b);

f32 vertex_magnitude(Vertex v);

Vertex vertex_normal(Vertex v);

#endif
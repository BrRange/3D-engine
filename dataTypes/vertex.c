#include "vertex.h"
#include <math.h>

Vertex vertex_new(f32 x, f32 y, f32 z){
  Vertex v = {
    .x = x,
    .y = y,
    .z = z
  };
  return v;
}

Vertex vertex_getClip(Vertex clip, Vertex unclip, f32 z){
  Vertex v = {.z = z};
  f32
    dx = unclip.x - clip.x,
    dy = unclip.y - clip.y,
    dz = unclip.z - clip.z;
  if(dz == 0.f) dz = 1e-6f;
  v.x = dx / dz * (z - clip.z) + clip.x;
  v.y = dy / dz * (z - clip.z) + clip.y;
  return v;
}

int vertex_projectionCompare(Vertex *vert1, Vertex *vert2){
  return vert2->z - vert1->z;
}

Vertex vertex_add(Vertex a, Vertex b){
  return vertex_new(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vertex vertex_sub(Vertex a, Vertex b){
  return vertex_new(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vertex vertex_scalarMul(Vertex a, f32 scalar){
  return vertex_new(a.x * scalar, a.y * scalar, a.z * scalar);
}

Vertex vertex_scalarDiv(Vertex a, f32 scalar){
  return vertex_new(a.x / scalar, a.y / scalar, a.z / scalar);
}

f32 vertex_dot(Vertex a, Vertex b){
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vertex vertex_cross(Vertex a, Vertex b){
  return vertex_new(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

f32 vertex_magnitude(Vertex a){
  return hypotf(hypotf(a.x, a.y), a.z);
}

Vertex vertex_normal(Vertex v){
  float scale = vertex_magnitude(v);
  return vertex_scalarDiv(v, scale);
}
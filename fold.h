#ifndef FOLD_H_
#define FOLD_H_

#include <SDL3/SDL.h>
#include "rustydef.h"

union Vertex{
  f32 coord[3];
  struct{
    f32 x, y, z;
  };
};
typedef union Vertex Vertex;

Vertex vertex_new(f32 x, f32 y, f32 z);

Vertex vertex_rotX(Vertex *v, f32 s, f32 c);

Vertex vertex_rotY(Vertex *v, f32 s, f32 c);

Vertex vertex_rotZ(Vertex *v, f32 s, f32 c);

Vertex vertex_rotate(Vertex *v, Vertex angles);

Vertex vertex_getClip(Vertex clip, Vertex unclip, f32 z);

int vertex_projectionCompare(Vertex *vert1, Vertex *vert2);

/* Vertex operators */

Vertex vertex_add(Vertex a, Vertex b);

Vertex vertex_sub(Vertex a, Vertex b);

Vertex vertex_scalarMul(Vertex a, f32 scalar);

Vertex vertex_scalarDiv(Vertex a, f32 scalar);

f32 vertex_dot(Vertex a, Vertex b);

Vertex vertex_cross(Vertex a, Vertex b);

f32 vertex_magnitude(Vertex a);

struct Camera{
  Vertex pos;
  f32 farPlane, nearPlane, fieldView;
  f32 yaw, pitch;
};
typedef struct Camera Camera;

Camera camera(Vertex v, f32 farPlane, f32 nearPlane, f32 fieldView);

Vertex camera_viewVertex(const Camera *cam);

void camera_rotate(Camera *cam, f32 dYaw, f32 dPitch);

void camera_rotateUnbound(Camera *cam, f32 dYaw, f32 dPitch);

void camera_moveAbs(Camera *cam, Vertex dPos);

void camera_moveRel(Camera *cam, Vertex dPos);

Vertex vertex_onCamera(const Vertex *restrict v, const Camera *restrict cam, const Vertex offset, const Vertex rot, f32 scale);

union Color{
  f32 rgba[4];
  struct{
    f32 r, g, b, a;
  };
};
typedef union Color Color;

Color color(f32 r, f32 g, f32 b, f32 a);

struct Polygon{
  u16 idx[3];
  u16 colorIndex;
};
typedef struct Polygon Polygon;

Polygon polygon_new(u16 idx0, u16 idx1, u16 idx2, u16 colorIndex);

struct Model{
  Vertex *vertex;
  size_t vertexCount;
  Polygon *polygon;
  size_t polyCount;
};
typedef struct Model Model;

Model model(Vertex *vertex_new, size_t vertexCount, Polygon *polygon_new, size_t polyCount);

struct Object{
  Model *model;
  Color *palette;
  Vertex pos, rot;
  f32 scale;
};
typedef struct Object Object;

Object object_new(Model *model, Color *palette, Vertex rCenter, f32 scale);

void object_rotateX(Object *obj, f32 ang);

void object_rotateY(Object *obj, f32 ang);

void object_rotateZ(Object *obj, f32 ang);

void object_move(Object *obj, Vertex dv);

void object_render(Object *obj, SDL_Renderer *rend, Camera *cam, f32 cx, f32 cy);

int object_distCompare(void **ref, u32 *idx1, u32 *idx2);

#endif
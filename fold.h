#ifndef FOLD_H_
#define FOLD_H_

#include <SDL3/SDL.h>
#include "dataTypes/quaternion.h"

struct Camera{
  Quaternion rot;
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

Vertex vertex_onCamera(const Vertex v, const Camera *restrict cam, const Vertex offset, const Quaternion rot, f32 scale);

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
  Quaternion rot;
  Vertex pos;
  f32 scale;
};
typedef struct Object Object;

Object object_new(Model *model, Color *palette, Vertex rCenter, f32 scale);

void object_rotate(Object *obj, Quaternion quat);

void object_move(Object *obj, Vertex dv);

void object_render(Object *obj, SDL_Renderer *rend, Camera *cam, f32 cx, f32 cy);

int object_distCompare(void **ref, u32 *idx1, u32 *idx2);

#endif
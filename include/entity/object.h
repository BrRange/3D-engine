#ifndef OBJECT_H_
#define OBJECT_H_

#include "dataType/canvas.h"
#include "entity/camera.h"

struct Polygon{
  Vec4 normal;
  Vec2 uv[3];
  u16 idx[3];
};
typedef struct Polygon Polygon;

Polygon polygon_new(Vec4 normal, Vec2 *uv, u16 *idx);

struct Model{
  Vec4 *vert;
  Polygon *polygon;
  size_t polyCount;
};
typedef struct Model Model;

Model model_new(Vec4 *vert, Polygon *polygon, size_t polyCount);

struct Object{
  Model *model;
  SDL_Surface *UVmap;
  Quaternion rot;
  Vec4 pos;
  f32 scale;
};
typedef struct Object Object;

Object object_new(Model *model, SDL_Surface *UVmap, const Vec4 rCenter, f32 scale);

void object_rotate(Object *obj, Quaternion quat);

void object_move(Object *obj, const Vec4 dv);

void object_render(Object *obj, Canvas *canv, Camera *cam);

#endif
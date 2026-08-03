#ifndef OBJECT_H_
#define OBJECT_H_

#include "dataType/canvas.h"
#include "entity/camera.h"
#include "entity/lightSource.h"

struct Polygon{
  Vec3 normal;
  Vec2 uv[3];
  u16 idx[3];
};
typedef struct Polygon Polygon;

Polygon polygon_new(Vec3 normal, Vec2 *uv, u16 *idx);

struct Model{
  Vec3 *vec3;
  Polygon *polygon;
  size_t polyCount;
};
typedef struct Model Model;

Model model_new(Vec3 *vert, Polygon *polygon, size_t polyCount);

struct Object{
  Model *model;
  SDL_Surface *UVmap;
  Quaternion rot;
  Vec3 pos;
  f32 scale;
};
typedef struct Object Object;

Object object_new(Model *model, SDL_Surface *UVmap, const Vec3 rCenter, f32 scale);

void object_rotate(Object *obj, Quaternion quat);

void object_move(Object *obj, const Vec3 dv);

void object_render(Object *obj, Canvas *canv, Camera *cam, LightSource_Packed *sources);

#endif
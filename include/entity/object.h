#ifndef OBJECT_H_
#define OBJECT_H_

#include "dataType/canvas.h"
#include "entity/camera.h"
#include "entity/lightSource.h"

struct Polygon{
  u16 idx[3];
  u16 colorIndex;
};
typedef struct Polygon Polygon;

Polygon polygon_new(u16 idx0, u16 idx1, u16 idx2, u16 colorIndex);

struct Model{
  Vec3 *vec3;
  size_t vec3Count;
  Polygon *polygon;
  size_t polyCount;
};
typedef struct Model Model;

Model model(Vec3 *vert, size_t vertCount, Polygon *polygon, size_t polyCount);

struct Object{
  Model *model;
  Color *palette;
  Quaternion rot;
  Vec3 pos;
  f32 scale;
};
typedef struct Object Object;

Object object_new(Model *model, Color *palette, const Vec3 rCenter, f32 scale);

void object_rotate(Object *obj, Quaternion quat);

void object_move(Object *obj, const Vec3 dv);

void object_render(Object *obj, Canvas *canv, Camera *cam, LightSource_Packed *sources);

#endif
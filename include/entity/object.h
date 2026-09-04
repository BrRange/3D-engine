#ifndef OBJECT_H_
#define OBJECT_H_

#include "dataType/canvas.h"
#include "entity/camera.h"

struct Vertex{
  Vec4 coord, normal;
  Vec2 uv;
};
typedef struct Vertex Vertex;

Vertex vertex_new(Vec4 coord, Vec4 normal, Vec2 uv);

Vertex vertex_mix(const Vertex a, const Vertex b, f32 t);

struct Model{
  Vertex *vert;
  Polygon *polygon;
  size_t polyCount;
};
typedef struct Model Model;

Model model_new(Vertex *vert, Polygon *polygon, size_t polyCount);

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
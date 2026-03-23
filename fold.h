#ifndef FOLD_H_
#define FOLD_H_

#include <SDL3/SDL.h>
#include "dataTypes/quaternion.h"

union Color{
  u32 rgba;
  struct{
    u8 r, g, b, a;
  };
};
typedef union Color Color;

typedef struct Canvas{
  Color *pixel;
  f32 *zBuffer;
  SDL_Texture *tex;
  u32 w, h;
} Canvas;

struct Camera{
  Quaternion rot;
  Vec3 pos;
  f32 farPlane, nearPlane, fieldView;
  f32 yaw, pitch;
};
typedef struct Camera Camera;

Camera camera(Vec3 v, f32 farPlane, f32 nearPlane, f32 fieldView);

Vec3 camera_viewVec3(const Camera *cam);

void camera_rotate(Camera *cam, f32 dYaw, f32 dPitch);

void camera_rotateUnbound(Camera *cam, f32 dYaw, f32 dPitch);

void camera_moveAbs(Camera *cam, Vec3 dPos);

void camera_moveRel(Camera *cam, Vec3 dPos);

Vec3 vec3_onCamera(const Vec3 v, const Camera *restrict cam, const Vec3 offset, const Quaternion rot, f32 scale);

Color color(u8 r, u8 g, u8 b, u8 a);

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

Model model(Vec3 *vec3_new, size_t vec3Count, Polygon *polygon_new, size_t polyCount);

struct Object{
  Model *model;
  Color *palette;
  Quaternion rot;
  Vec3 pos;
  f32 scale;
};
typedef struct Object Object;

Object object_new(Model *model, Color *palette, Vec3 rCenter, f32 scale);

void object_rotate(Object *obj, Quaternion quat);

void object_move(Object *obj, Vec3 dv);

void object_render(Object *obj, Canvas *canv, Camera *cam);

Canvas canvas_new(SDL_Renderer *rend, u32 w, u32 h);

void canvas_destroy(Canvas *canv);

#endif
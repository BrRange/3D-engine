#ifndef VECTOR_H_
#define VECTOR_H_

#include "rustydef.h"

typedef struct Vec3{
  f32 x, y, z;
} Vec3;

Vec3 vec3_new(f32 x, f32 y, f32 z);

Vec3 vec3_getClip(Vec3 clip, Vec3 unclip, f32 z);

int vec3_projectionCompare(Vec3 *a, Vec3 *b);

/* Vec3 operators */

Vec3 vec3_add(Vec3 a, Vec3 b);

Vec3 vec3_sub(Vec3 a, Vec3 b);

Vec3 vec3_mul(Vec3 v, f32 scalar);

Vec3 vec3_div(Vec3 v, f32 scalar);

f32 vec3_dot(Vec3 a, Vec3 b);

Vec3 vec3_cross(Vec3 a, Vec3 b);

f32 vec3_mag(Vec3 v);

Vec3 vec3_normal(Vec3 v);

typedef struct Vec2{
  f32 x, y;
} Vec2;

Vec2 vec2_new(f32 x, f32 y);

/* Vec2 operators */

Vec2 vec2_add(Vec2 a, Vec2 b);

Vec2 vec2_sub(Vec2 a, Vec2 b);

Vec2 vec2_mul(Vec2 v, f32 scalar);

Vec2 vec2_div(Vec2 v, f32 scalar);

f32 vec2_dot(Vec2 a, Vec2 b);

Vec2 vec2_perp(Vec2 v);

f32 vec2_mag(Vec2 v);

Vec2 vec2_normal(Vec2 v);

f32 vec2_edge(Vec2 a, Vec2 b, Vec2 c);

void vec2_bound(Vec2 a, Vec2 b, Vec2 c, i32 bounds[4]);

#endif
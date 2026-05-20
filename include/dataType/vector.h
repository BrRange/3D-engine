#ifndef DATATYPE_VECTOR_H_
#define DATATYPE_VECTOR_H_

#include "dataType/rustydef.h"

typedef struct Vec3{
  f32 x, y, z, w;
} Vec3;

Vec3 vec3_new(f32 x, f32 y, f32 z);

Vec3 vec3_getClip(const Vec3 clip, const Vec3 unclip, f32 z);

/* Vec3 operators */

Vec3 vec3_add(const Vec3 a, const Vec3 b);

Vec3 vec3_sub(const Vec3 a, const Vec3 b);

Vec3 vec3_mul(const Vec3 v, f32 scalar);

Vec3 vec3_div(const Vec3 v, f32 scalar);

f32 vec3_dot(const Vec3 a, const Vec3 b);

Vec3 vec3_cross(const Vec3 a, const Vec3 b);

f32 vec3_mag(const Vec3 v);

Vec3 vec3_normal(const Vec3 v);

Vec3 vec3_piecewise(const Vec3 a, const Vec3 b);

Vec3 vec3_expand(float f);

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

Vec2 vec2_piecewise(Vec2 a, Vec2 b);

Vec2 vec2_expand(float f);

void vec2_bound(Vec2 a, Vec2 b, Vec2 c, i32 bounds[4]);

#endif
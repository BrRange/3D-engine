#ifndef DATATYPE_VECTOR_H_
#define DATATYPE_VECTOR_H_

#include "dataType/rustydef.h"

typedef f32 __attribute__((vector_size(16))) Vec4;

Vec4 vec4_new(f32 x, f32 y, f32 z);

f32 vec4_getClip(const Vec4 clip, const Vec4 unclip, f32 z);

/* Vec3 operators */

f32 vec4_dot(const Vec4 a, const Vec4 b);

Vec4 vec4_cross(const Vec4 a, const Vec4 b);

f32 vec4_mag(const Vec4 v);

Vec4 vec4_normal(const Vec4 v);

Vec4 vec4_expand(f32 f);

Vec4 vec4_mix(const Vec4 a, const Vec4 b, f32 f);

typedef f32 __attribute__((vector_size(64))) Mat4;

Mat4 mat4_compose(const Mat4 a, const Mat4 b){
  Mat4 res = {0};
  for(int i = 0; i < 4; ++i)
  for(int k = 0; k < 4; ++k)
  for(int j = 0; j < 4; ++j)
  res[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
  return res;
}

Vec4 vec4_compose(const Mat4 m, Vec4 v){
  Vec4 res = {0};
  for(int i = 0; i < 4; ++i)
  for(int j = 0; j < 4; ++j)
  res[i] += m[i * 4 + j] * v[j];
  return res;
}

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
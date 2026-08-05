#include <math.h>
#include <SDL3/SDL_stdinc.h>
#include "dataType/vector.h"

Vec4 vec4_new(f32 x, f32 y, f32 z){
  return (Vec4){x, y, z};
}

Vec4 vec4_getClip(const Vec4 clip, const Vec4 unclip, f32 z){
  Vec4 v = {0, 0, z};
  Vec4 dv = unclip - clip;
  if(dv[2] == 0.f) dv[2] = 1e-6f;
  v[0] = dv[0] / dv[2] * (z - clip[2]) + clip[0];
  v[1] = dv[1] / dv[2] * (z - clip[2]) + clip[1];
  return v;
}

f32 vec4_dot(const Vec4 a, const Vec4 b){
  Vec4 d = a * b;
  return d[0] + d[1] + d[2];
}

Vec4 vec4_cross(const Vec4 a, const Vec4 b){
  return vec4_new(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
}

f32 vec4_mag(const Vec4 v){
  return hypotf(v[0], hypotf(v[1], v[2]));
}

Vec4 vec4_normal(const Vec4 v){
  f32 m = vec4_mag(v);
  if(!m) return vec4_expand(0.f);
  return v / m;
}

Vec4 vec4_expand(f32 f){
  return vec4_new(f, f, f);
}

Vec4 vec4_mix(const Vec4 a, const Vec4 b, f32 t){
  return a * (1.f - t) + b * t;
}

Vec2 vec2_new(f32 x, f32 y){
  Vec2 v = {.x = x, .y = y};
  return v;
}

Vec2 vec2_add(Vec2 a, Vec2 b){
  a.x += b.x;
  a.y += b.y;
  return a;
}

Vec2 vec2_sub(Vec2 a, Vec2 b){
  a.x -= b.x;
  a.y -= b.y;
  return a;
}

Vec2 vec2_mul(Vec2 v, f32 scalar){
  v.x *= scalar;
  v.y *= scalar;
  return v;
}

Vec2 vec2_div(Vec2 v, f32 scalar){
  v.x /= scalar;
  v.y /= scalar;
  return v;
}

f32 vec2_dot(Vec2 a, Vec2 b){
  return a.x * b.x + a.y * b.y;
}

Vec2 vec2_perp(Vec2 v){
  Vec2 p = {.x = -v.y, .y = v.x};
  return p;
}

f32 vec2_mag(Vec2 v){
  return hypotf(v.x, v.y);
}

Vec2 vec2_normal(Vec2 v){
  f32 mag = vec2_mag(v);
  if(mag == 0.f) return vec2_expand(0);
  return vec2_div(v, mag);
}

f32 vec2_edge(Vec2 a, Vec2 b, Vec2 c){
  return (b.x - a.x) * (a.y - c.y) + (b.y - a.y) * (c.x - a.x);
}

Vec2 vec2_piecewise(Vec2 a, Vec2 b){
  return vec2_new(a.x * b.x, a.y * b.y);
}

Vec2 vec2_expand(float f){
  return vec2_new(f, f);
}

void vec2_bound(Vec2 a, Vec2 b, Vec2 c, i32 bounds[4]){
  bounds[0] = SDL_min(a.x, SDL_min(b.x, c.x));
  bounds[1] = SDL_max(a.x, SDL_max(b.x, c.x));
  bounds[2] = SDL_min(a.y, SDL_min(b.y, c.y));
  bounds[3] = SDL_max(a.y, SDL_max(b.y, c.y));
  bounds[1] += 1;
  bounds[3] += 1;
}

Vec2 vec2_mix(const Vec2 a, const Vec2 b, f32 t){
  return vec2_add(vec2_mul(a, (1.f - t)), vec2_mul(b, t));
}
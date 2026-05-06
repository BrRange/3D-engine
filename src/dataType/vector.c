#include <math.h>
#include <SDL3/SDL_stdinc.h>
#include "dataType/vector.h"

Vec3 vec3_new(f32 x, f32 y, f32 z){
  Vec3 v = {
    .x = x,
    .y = y,
    .z = z
  };
  return v;
}

Vec3 vec3_getClip(Vec3 clip, Vec3 unclip, f32 z){
  Vec3 v = {.z = z};
  f32
    dx = unclip.x - clip.x,
    dy = unclip.y - clip.y,
    dz = unclip.z - clip.z;
  if(dz == 0.f) dz = 1e-6f;
  v.x = dx / dz * (z - clip.z) + clip.x;
  v.y = dy / dz * (z - clip.z) + clip.y;
  return v;
}

int vec3_projectionCompare(Vec3 *vert1, Vec3 *vert2){
  return vert2->z - vert1->z;
}

Vec3 vec3_add(Vec3 a, Vec3 b){
  return vec3_new(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vec3_sub(Vec3 a, Vec3 b){
  return vec3_new(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 vec3_mul(Vec3 a, f32 scalar){
  return vec3_new(a.x * scalar, a.y * scalar, a.z * scalar);
}

Vec3 vec3_div(Vec3 a, f32 scalar){
  return vec3_new(a.x / scalar, a.y / scalar, a.z / scalar);
}

f32 vec3_dot(Vec3 a, Vec3 b){
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 vec3_cross(Vec3 a, Vec3 b){
  return vec3_new(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

f32 vec3_mag(Vec3 a){
  return hypotf(hypotf(a.x, a.y), a.z);
}

Vec3 vec3_normal(Vec3 v){
  float scale = vec3_mag(v);
  if(scale == 0.f) return vec3_expand(0);
  return vec3_div(v, scale);
}

Vec3 vec3_piecewise(Vec3 a, Vec3 b){
  return vec3_new(a.x * b.x, a.y * b.y, a.z * b.z);
}

Vec3 vec3_expand(float f){
  return vec3_new(f, f, f);
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
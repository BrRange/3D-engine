#include "include/entity/collider.h"

Collider_Sphere collider_newSphere(Object *anchor, Vec3 offset, float radius){
  Collider_Sphere sphere = {
    .base = {
      .anchor = anchor,
      .type = ColliderType_Sphere,
      .offset = offset
    },
    .radius = radius
  };
  return sphere;
}

Collider_Pill collider_newPill(Object *anchor, Vec3 offset, Vec3 extension, float radius){
  Collider_Pill pill = {
    .base = {
      .anchor = anchor,
      .type = ColliderType_Pill,
      .offset = offset
    },
    .extension = extension,
    .radius = radius
  };
  return pill;
}

Collider_Beam collider_newBeam(Object *anchor, Vec3 offset, Vec3 dir, float maxDist){
  Collider_Beam beam = {
    .base = {
      .anchor = anchor,
      .type = ColliderType_Beam,
      .offset = offset
    },
    .dir = dir,
    .maxDist = maxDist
  };
  return beam;
}

bool collider_collide(Collider *a, Collider *b){
  if(a->type == ColliderType_Intangible || b->type == ColliderType_Intangible) return false;

  switch(a->type){
    case ColliderType_Sphere:
    switch(b->type){
      case ColliderType_Sphere:
      return collider_sphere_sphere((Collider_Sphere*)a, (Collider_Sphere*)b);
      case ColliderType_Pill:
      return collider_sphere_pill((Collider_Sphere*)a, (Collider_Pill*)b);
      case ColliderType_Beam:
      return collider_sphere_beam((Collider_Sphere*)a, (Collider_Beam*)b);
    } break;

    case ColliderType_Pill:
    switch(b->type){
      case ColliderType_Sphere:
      return collider_sphere_pill((Collider_Sphere*)b, (Collider_Pill*)a);
      case ColliderType_Pill:
      return collider_pill_pill((Collider_Pill*)a, (Collider_Pill*)b);
      case ColliderType_Beam:
      return collider_pill_beam((Collider_Pill*)a, (Collider_Beam*)b);
    } break;
    
    case ColliderType_Beam:
    switch(b->type){
      case ColliderType_Sphere:
      return collider_sphere_beam((Collider_Sphere*)b, (Collider_Beam*)a);
      case ColliderType_Pill:
      return collider_pill_beam((Collider_Pill*)b, (Collider_Beam*)a);
      case ColliderType_Beam:
      return collider_beam_beam((Collider_Beam*)a, (Collider_Beam*)b);
    } break;
  }
  return false;
}

bool collider_sphere_sphere(Collider_Sphere *a, Collider_Sphere *b){
  Vec3 aPos, bPos;

  aPos = vec3_add(a->base.anchor->pos, a->base.offset);
  aPos = vec3_rotate(aPos, a->base.anchor->rot);

  bPos = vec3_add(b->base.anchor->pos, b->base.offset);
  bPos = vec3_rotate(bPos, b->base.anchor->rot);

  Vec3 diff = vec3_sub(aPos, bPos);
  float radius = a->radius + b->radius;
  return vec3_dot(diff, diff) <= radius * radius;
}

bool collider_sphere_pill(Collider_Sphere *sphere, Collider_Pill *pill){
  Vec3 spherePos, pillPos[2];
  
  spherePos = vec3_add(sphere->base.anchor->pos, sphere->base.offset);
  spherePos = vec3_rotate(spherePos, sphere->base.anchor->rot);

  pillPos[0] = vec3_add(pill->base.anchor->pos, pill->base.offset);
  pillPos[1] = vec3_add(pill->base.anchor->pos, pill->extension);
  pillPos[0] = vec3_rotate(pillPos[0], pill->base.anchor->rot);
  pillPos[1] = vec3_rotate(pillPos[1], pill->base.anchor->rot);
  
  Vec3 line = vec3_sub(pillPos[0], pillPos[1]);
  Vec3 proj = vec3_sub(pillPos[0], spherePos);

  float radius = vec3_dot(line, proj) / vec3_dot(line, line);
  radius = SDL_clamp(radius, 0.f, 1.f);
  line = vec3_add(vec3_mul(pillPos[0], 1.f - radius), vec3_mul(pillPos[1], radius));

  proj = vec3_sub(line, spherePos);
  radius = sphere->radius + pill->radius;

  return vec3_dot(proj, proj) <= radius * radius;
}

bool collider_sphere_beam(Collider_Sphere *sphere, Collider_Beam *beam){
  Vec3 spherePos, beamPos;

  spherePos = vec3_add(sphere->base.anchor->pos, sphere->base.offset);
  spherePos = vec3_rotate(spherePos, sphere->base.anchor->rot);

  beamPos = vec3_add(sphere->base.anchor->pos, sphere->base.offset);
  beamPos = vec3_rotate(spherePos, sphere->base.anchor->rot);
  beamPos = vec3_sub(beamPos, spherePos);

  float angularDist, linearDist = vec3_dot(beamPos, beamPos), radius = sphere->radius;
  radius *= radius;

  if(linearDist <= radius) return true;
  angularDist = vec3_dot(beamPos, beam->dir);

  float parallelism = angularDist / linearDist;
  if(parallelism >= 0) return false;

  return linearDist - radius <= angularDist * parallelism;
}

bool collider_pill_pill(Collider_Pill *a, Collider_Pill *b){
  (void) a;
  (void) b;
  return false;
}

bool collider_pill_beam(Collider_Pill *pill, Collider_Beam *beam){
  (void) pill;
  (void) beam;
  return false;
}

bool collider_beam_beam(Collider_Beam *a, Collider_Beam *b){
  (void) a;
  (void) b;
  return false;
}
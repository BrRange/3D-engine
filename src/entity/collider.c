#include "entity/collider.h"

#include <SDL3/SDL_assert.h>
#define UNIMPLEMENTED false

Collider_Sphere collider_newSphere(Object *anchor, Vec3 offset, f32 radius){
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

Collider_Pill collider_newPill(Object *anchor, Vec3 offset, Vec3 extension, f32 radius){
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

Collider_Beam collider_newBeam(Object *anchor, Vec3 offset, Vec3 dir, f32 maxDist){
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

Collider_Box collider_newBox(Object *anchor, Vec3 offset, Vec3 extension){
  Collider_Box box = {
    .base = {
      .anchor = anchor,
      .type = ColliderType_Box,
      .offset = offset
    },
    .extension = extension
  };
  return box;
}

bool collider_collide(Collider *a, Collider *b, CollisionInfo *info){
  *info = (CollisionInfo){0};
  if(a->type == ColliderType_Intangible || b->type == ColliderType_Intangible) return false;

  switch(a->type){
    case ColliderType_Sphere:
    switch(b->type){
      case ColliderType_Sphere:
      return collider_sphere_sphere((Collider_Sphere*)a, (Collider_Sphere*)b, info);
      case ColliderType_Pill:
      return collider_sphere_pill((Collider_Sphere*)a, (Collider_Pill*)b, info);
      case ColliderType_Beam:
      return collider_sphere_beam((Collider_Sphere*)a, (Collider_Beam*)b, info);
      case ColliderType_Box:
      return collider_sphere_box((Collider_Sphere*)a, (Collider_Box*)b, info);
    } break;

    case ColliderType_Pill:
    switch(b->type){
      case ColliderType_Sphere:
      return collider_sphere_pill((Collider_Sphere*)b, (Collider_Pill*)a, info);
      case ColliderType_Pill:
      return collider_pill_pill((Collider_Pill*)a, (Collider_Pill*)b, info);
      case ColliderType_Beam:
      return collider_pill_beam((Collider_Pill*)a, (Collider_Beam*)b, info);
      case ColliderType_Box:
      return collider_pill_box((Collider_Pill*)a, (Collider_Box*)b, info);
    } break;
    
    case ColliderType_Beam:
    switch(b->type){
      case ColliderType_Sphere:
      return collider_sphere_beam((Collider_Sphere*)b, (Collider_Beam*)a, info);
      case ColliderType_Pill:
      return collider_pill_beam((Collider_Pill*)b, (Collider_Beam*)a, info);
      case ColliderType_Beam:
      return collider_beam_beam((Collider_Beam*)a, (Collider_Beam*)b, info);
      case ColliderType_Box:
      return collider_beam_box((Collider_Beam*)a, (Collider_Box*)b, info);
    } break;

    case ColliderType_Box:
    switch(b->type){
      case ColliderType_Sphere:
      return collider_sphere_box((Collider_Sphere*)b, (Collider_Box*)a, info);
      case ColliderType_Pill:
      return collider_pill_box((Collider_Pill*)b, (Collider_Box*)a, info);
      case ColliderType_Beam:
      return collider_beam_box((Collider_Beam*)a, (Collider_Box*)b, info);
      case ColliderType_Box:
      return collider_box_box((Collider_Box*)a, (Collider_Box*)b, info);
    } break;
  }
  return false;
}

bool collider_sphere_sphere(Collider_Sphere *a, Collider_Sphere *b, CollisionInfo *info){
  Vec3 aPos, bPos;

  aPos = vec3_rotate(a->base.offset, a->base.anchor->rot);
  aPos = vec3_add(a->base.anchor->pos, aPos);

  bPos = vec3_rotate(b->base.offset, b->base.anchor->rot);
  bPos = vec3_add(b->base.anchor->pos, bPos);

  Vec3 diff = vec3_sub(bPos, aPos);
  f32 radius = a->radius + b->radius;
  f32 sqrDist = vec3_dot(diff, diff);

  if(info){
    info->source = (Collider*)b;
    info->dest = (Collider*)b;
    info->normal = vec3_normal(diff);
    info->penetration = SDL_sqrtf(sqrDist) - radius;
  }

  return sqrDist <= radius * radius;
}

bool collider_sphere_pill(Collider_Sphere *sphere, Collider_Pill *pill, CollisionInfo *info){
  Vec3 spherePos, pillPos[2];
  
  spherePos = vec3_add(sphere->base.anchor->pos, sphere->base.offset);
  spherePos = vec3_rotate(spherePos, sphere->base.anchor->rot);

  pillPos[0] = vec3_rotate(pill->base.offset, pill->base.anchor->rot);
  pillPos[1] = vec3_rotate(pill->extension, pill->base.anchor->rot);
  pillPos[0] = vec3_add(pill->base.anchor->pos, pillPos[0]);
  pillPos[1] = vec3_add(pill->base.anchor->pos, pillPos[1]);
  
  Vec3 line = vec3_sub(pillPos[0], pillPos[1]);
  Vec3 proj = vec3_sub(pillPos[0], spherePos);

  f32 radius = vec3_dot(line, proj) / vec3_dot(line, line);
  radius = SDL_clamp(radius, 0.f, 1.f);
  line = vec3_add(vec3_mul(pillPos[0], 1.f - radius), vec3_mul(pillPos[1], radius));

  proj = vec3_sub(line, spherePos);
  radius = sphere->radius + pill->radius;

  f32 sqrDist = vec3_dot(proj, proj);

  if(info){
    info->source = (Collider*)sphere;
    info->dest = (Collider*)pill;
    info->normal = vec3_normal(proj);
    info->penetration = SDL_sqrtf(sqrDist) - radius;
  }

  return vec3_dot(proj, proj) <= radius * radius;
}

bool collider_sphere_beam(Collider_Sphere *sphere, Collider_Beam *beam, CollisionInfo *info){
  Vec3 spherePos, beamPos;

  spherePos = vec3_rotate(sphere->base.offset, sphere->base.anchor->rot);
  spherePos = vec3_add(sphere->base.anchor->pos, spherePos);

  beamPos = vec3_rotate(beam->dir, beam->base.anchor->rot);
  beamPos = vec3_add(beam->base.anchor->pos, beamPos);
  beamPos = vec3_sub(beamPos, spherePos);

  f32 angularDist, linearDist = vec3_dot(beamPos, beamPos), radius = sphere->radius;
  
  if(info){
    info->source = (Collider*)sphere;
    info->dest = (Collider*)beam;
    info->normal = beamPos;
    info->penetration = SDL_sqrtf(linearDist) - radius;
  }

  radius *= radius;
  if(linearDist <= radius) return true;
  angularDist = vec3_dot(beamPos, beam->dir);

  f32 parallelism = angularDist / linearDist;
  if(parallelism >= 0) return false;

  return linearDist - radius <= angularDist * parallelism;
}

bool collider_sphere_box(Collider_Sphere *sphere, Collider_Box *box, CollisionInfo *info){
  Vec3 spherePos = vec3_rotate(sphere->base.offset, sphere->base.anchor->rot);
  spherePos = vec3_add(spherePos, sphere->base.anchor->pos);

  Vec3 boxCenter = vec3_add(box->base.offset, box->base.anchor->pos);
  Vec3 boxExtent = box->extension;

  spherePos = vec3_sub(spherePos, boxCenter);
  spherePos = vec3_rotate(spherePos, quat_conjugate(box->base.anchor->rot));
  f32 dx = SDL_clamp(spherePos.x, -boxExtent.x, boxExtent.x);
  f32 dy = SDL_clamp(spherePos.y, -boxExtent.y, boxExtent.y);
  f32 dz = SDL_clamp(spherePos.z, -boxExtent.z, boxExtent.z);

  Vec3 diff = vec3_sub(vec3_new(dx, dy, dz), spherePos);
  f32 dist = SDL_sqrtf(vec3_dot(diff, diff));

  if(info){
    info->source = (Collider*)sphere;
    info->dest = (Collider*)box;
    info->normal = vec3_rotate(vec3_normal(diff),box->base.anchor->rot);
    info->penetration = dist - sphere->radius;
  }

  return dist <= sphere->radius;
}

bool collider_pill_pill(Collider_Pill *a, Collider_Pill *b, CollisionInfo *info){
  (void) a;
  (void) b;
  SDL_assert(UNIMPLEMENTED);
  return false;
}

bool collider_pill_beam(Collider_Pill *pill, Collider_Beam *beam, CollisionInfo *info){
  (void) pill;
  (void) beam;
  SDL_assert(UNIMPLEMENTED);
  return false;
}

bool collider_pill_box(Collider_Pill *pill, Collider_Box *box, CollisionInfo *info){
  (void) pill;
  (void) box;
  SDL_assert(UNIMPLEMENTED);
  return false;
}

bool collider_beam_beam(Collider_Beam *a, Collider_Beam *b, CollisionInfo *info){
  (void) a;
  (void) b;
  SDL_assert(UNIMPLEMENTED);
  return false;
}

bool collider_beam_box(Collider_Beam *beam, Collider_Box *box, CollisionInfo *info){
  (void) beam;
  (void) box;
  SDL_assert(UNIMPLEMENTED);
  return false;
}

bool collider_box_box(Collider_Box *a, Collider_Box *b, CollisionInfo *info){
  (void) a;
  (void) b;
  SDL_assert(UNIMPLEMENTED);
  return false;
}
#include "entity/collider.h"

#include <SDL3/SDL_assert.h>
#define UNIMPLEMENTED false

void collider_setResponse(Collider *coll, CollisionType type, f32 coef){
  coll->collision = type;
  coll->coef = coef;
}

Vec4 collision_getResponse(CollisionInfo *info, Vec4 displacement, f32 dt){
  switch(info->type){
    case CollisionType_Phase:{} break;
    case CollisionType_Slide:{
      Vec4 hover = info->normal * info->distance, norm = info->normal;
      f32 dragHover = vec4_dot(hover, norm), dragSpeed = vec4_dot(displacement, norm);
      displacement += hover;
      displacement -= norm * (dragHover + dragSpeed);
      displacement *= SDL_max(0.f, 1.f - info->coef * dt);
    } break;
    case CollisionType_Bounce:{
      f32 mag = vec4_mag(displacement);
      Vec4 scaled = info->normal * (mag * info->coef);
      displacement -= scaled;
    } break;
    case CollisionType_Sink:{
      f32 mag = vec4_mag(displacement);
      Vec4 scaled = info->normal * (mag * info->coef * dt);
      displacement -= scaled;
    } break;
  }
  return displacement;
}

void collision_defineType(CollisionInfo *info, Collider *a, Collider *b){
  if(!(a->collision && b->collision)){
    info->type = ColliderType_Intangible;
    info->coef = 0.f;
    return;
  }
  if(a->collision == b->collision){
    info->type = a->collision;
    info->coef = a->coef * b->coef;
    return;
  }
  if(a->collision > b->collision){
    info->type = a->collision;
    info->coef = a->coef;
  } else{
    info->type = b->collision;
    info->coef = b->coef;
  }
}

Collider_Sphere collider_newSphere(Object *anchor, Vec4 offset, f32 radius){
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

Collider_Pill collider_newPill(Object *anchor, Vec4 offset, Vec4 extension, f32 radius){
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

Collider_Beam collider_newBeam(Object *anchor, Vec4 offset, Vec4 dir, f32 maxDist){
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

Collider_Box collider_newBox(Object *anchor, const Vec4 offset, const Vec4 extension, const Vec4 normal){
  Collider_Box box = {
    .base = {
      .anchor = anchor,
      .type = ColliderType_Box,
      .offset = offset,
    },
    .extension = extension,
    .normal = normal
  };
  return box;
}

bool collider_collide(Collider *a, Collider *b, CollisionInfo *info){
  if(a->type == ColliderType_Intangible || b->type == ColliderType_Intangible) return false;
  if(info){
    *info = (CollisionInfo){0};
    collision_defineType(info, a, b);
    if(info->type == CollisionType_Slide || info->type == CollisionType_Bounce)
    info->snap = true;
  }

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
  Vec4 aPos, bPos;

  aPos = vec4_rotate(a->base.offset, a->base.anchor->rot);
  aPos += a->base.anchor->pos;

  bPos = vec4_rotate(b->base.offset, b->base.anchor->rot);
  bPos += b->base.anchor->pos;

  Vec4 diff = bPos - aPos;
  f32 radius = a->radius + b->radius;
  f32 sqrDist = vec4_dot(diff, diff);

  if(info){
    info->source = (Collider*)a;
    info->dest = (Collider*)b;
    info->normal = vec4_normal(diff);
    info->distance = sqrtf(sqrDist);
    info->penetration = info->distance - radius;
  }

  return sqrDist < radius * radius;
}

bool collider_sphere_pill(Collider_Sphere *sphere, Collider_Pill *pill, CollisionInfo *info){
  Vec4 spherePos, pillPos[2];
  
  spherePos = sphere->base.anchor->pos + sphere->base.offset;
  spherePos = vec4_rotate(spherePos, sphere->base.anchor->rot);

  pillPos[0] = vec4_rotate(pill->base.offset, pill->base.anchor->rot);
  pillPos[1] = vec4_rotate(pill->extension, pill->base.anchor->rot);
  pillPos[0] += pill->base.anchor->pos;
  pillPos[1] += pill->base.anchor->pos;
  
  Vec4 line = pillPos[0] - pillPos[1];
  Vec4 proj = pillPos[0] - spherePos;

  f32 radius = vec4_dot(line, proj) / vec4_dot(line, line);
  radius = SDL_clamp(radius, 0.f, 1.f);
  line = vec4_mix(pillPos[0], pillPos[1], radius);

  proj = line - spherePos;
  radius = sphere->radius + pill->radius;

  f32 sqrDist = vec4_dot(proj, proj);

  if(info){
    info->source = (Collider*)sphere;
    info->dest = (Collider*)pill;
    info->normal = vec4_normal(proj);
    info->distance = sqrtf(sqrDist);
    info->penetration = info->distance - radius;
  }

  return sqrDist < radius * radius;
}

bool collider_sphere_beam(Collider_Sphere *sphere, Collider_Beam *beam, CollisionInfo *info){
  Vec4 spherePos, beamPos;

  spherePos = vec4_rotate(sphere->base.offset, sphere->base.anchor->rot);
  spherePos += sphere->base.anchor->pos;

  beamPos = vec4_rotate(beam->dir, beam->base.anchor->rot);
  beamPos += beam->base.anchor->pos;
  beamPos -= spherePos;

  f32 angularDist, linearDist = vec4_dot(beamPos, beamPos), radius = sphere->radius;
  
  if(info){
    info->source = (Collider*)sphere;
    info->dest = (Collider*)beam;
    info->normal = beamPos;
    info->distance = sqrtf(linearDist);
    info->penetration = info->distance - radius;
  }

  radius *= radius;
  if(linearDist < radius) return true;
  angularDist = vec4_dot(beamPos, beam->dir);

  f32 parallelism = angularDist / linearDist;
  if(parallelism >= 0) return false;

  return linearDist - radius < angularDist * parallelism;
}

bool collider_sphere_box(Collider_Sphere *sphere, Collider_Box *box, CollisionInfo *info){
  Vec4 spherePos = vec4_rotate(sphere->base.offset, sphere->base.anchor->rot);
  spherePos += sphere->base.anchor->pos;

  Vec4 boxCenter = box->base.offset + box->base.anchor->pos;
  Vec4 boxExtent = box->extension;

  spherePos -= boxCenter;
  spherePos = vec4_rotate(spherePos, quat_conjugate(box->base.anchor->rot));
  f32 dx = SDL_clamp(spherePos[0], -boxExtent[0], boxExtent[0]);
  f32 dy = SDL_clamp(spherePos[1], -boxExtent[1], boxExtent[1]);
  f32 dz = SDL_clamp(spherePos[2], -boxExtent[2], boxExtent[2]);

  Vec4 diff = vec4_new(dx, dy, dz) - spherePos;
  f32 dist = vec4_mag(diff);

  if(info){
    info->source = (Collider*)sphere;
    info->dest = (Collider*)box;
    if(dist == 0.f) info->normal = vec4_rotate(box->normal, box->base.anchor->rot);
    else info->normal = vec4_rotate(vec4_normal(diff), box->base.anchor->rot);
    info->distance = dist;
    info->penetration = dist - sphere->radius;
  }

  return dist < sphere->radius;
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
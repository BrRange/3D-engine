#include <math.h>
#include "collider.h"

Collider_Sphere collider_newSphere(Object *anchor, Vertex offset, float radius){
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

Collider_Pill collider_newPill(Object *anchor, Vertex offset, Vertex extension, float radius){
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

Collider_Beam collider_newBeam(Object *anchor, Vertex offset, Vertex dir, float maxDist){
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
    }

    case ColliderType_Pill:
    switch(b->type){
      case ColliderType_Sphere:
      return collider_sphere_pill((Collider_Sphere*)b, (Collider_Pill*)a);
      case ColliderType_Pill:
      return collider_pill_pill((Collider_Pill*)a, (Collider_Pill*)b);
      case ColliderType_Beam:
      return collider_pill_beam((Collider_Pill*)a, (Collider_Beam*)b);
    }
    
    case ColliderType_Beam:
    switch(b->type){
      case ColliderType_Sphere:
      return collider_sphere_beam((Collider_Sphere*)b, (Collider_Beam*)a);
      case ColliderType_Pill:
      return collider_pill_beam((Collider_Pill*)b, (Collider_Beam*)a);
      case ColliderType_Beam:
      return collider_beam_beam((Collider_Beam*)a, (Collider_Beam*)b);
    }
  }
  return false;
}

bool collider_sphere_sphere(Collider_Sphere *a, Collider_Sphere *b){
  float s, c;
  Vertex aPos, bPos;

  aPos = vertex_add(a->base.anchor->pos, a->base.offset);
  sincosf(a->base.anchor->rot.z, &s, &c);
  aPos = vertex_rotZ(&aPos, s, c);
  sincosf(a->base.anchor->rot.y, &s, &c);
  aPos = vertex_rotY(&aPos, s, c);
  sincosf(a->base.anchor->rot.x, &s, &c);
  aPos = vertex_rotX(&aPos, s, c);

  bPos = vertex_add(b->base.anchor->pos, b->base.offset);
  sincosf(b->base.anchor->rot.z, &s, &c);
  bPos = vertex_rotX(&bPos, s, c);
  sincosf(b->base.anchor->rot.y, &s, &c);
  bPos = vertex_rotY(&bPos, s, c);
  sincosf(b->base.anchor->rot.x, &s, &c);
  bPos = vertex_rotZ(&bPos, s, c);

  Vertex diff = vertex_sub(aPos, bPos);
  float radius = a->radius + b->radius;
  return vertex_dot(diff, diff) <= radius * radius;
}

bool collider_sphere_pill(Collider_Sphere *sphere, Collider_Pill *pill){
  float s, c;
  Vertex spherePos, pillPos[2];
  
  spherePos = vertex_add(sphere->base.anchor->pos, sphere->base.offset);
  spherePos = vertex_rotate(&spherePos, sphere->base.anchor->rot);

  pillPos[0] = vertex_add(pill->base.anchor->pos, pill->base.offset);
  pillPos[1] = vertex_add(pill->base.anchor->pos, pill->extension);
  sincosf(pill->base.anchor->rot.z, &s, &c);
  pillPos[0] = vertex_rotZ(pillPos + 0, s, c);
  pillPos[1] = vertex_rotZ(pillPos + 1, s, c);
  sincosf(pill->base.anchor->rot.y, &s, &c);
  pillPos[0] = vertex_rotY(pillPos + 0, s, c);
  pillPos[1] = vertex_rotY(pillPos + 1, s, c);
  sincosf(pill->base.anchor->rot.x, &s, &c);
  pillPos[0] = vertex_rotX(pillPos + 0, s, c);
  pillPos[1] = vertex_rotX(pillPos + 1, s, c);
  
  Vertex line = vertex_sub(pillPos[0], pillPos[1]);
  Vertex proj = vertex_sub(pillPos[0], spherePos);

  float radius = vertex_dot(line, proj) / vertex_dot(line, line);
  radius = SDL_clamp(radius, 0.f, 1.f);
  line = vertex_add(vertex_scalarMul(pillPos[0], 1.f - radius), vertex_scalarMul(pillPos[1], radius));

  proj = vertex_sub(line, spherePos);
  radius = sphere->radius + pill->radius;

  return vertex_dot(proj, proj) <= radius * radius;
}

bool collider_sphere_beam(Collider_Sphere *sphere, Collider_Beam *beam){
  Vertex spherePos, beamPos;

  spherePos = vertex_add(sphere->base.anchor->pos, sphere->base.offset);
  spherePos = vertex_rotate(&sphere, sphere->base.anchor->rot);

  beamPos = vertex_add(sphere->base.anchor->pos, sphere->base.offset);
  beamPos = vertex_rotate(&sphere, sphere->base.anchor->rot);
  beamPos = vertex_sub(beamPos, spherePos);

  float angularDist, linearDist = vertex_dot(beamPos, beamPos), radius = sphere->radius;
  radius *= radius;

  if(linearDist <= radius) return true;
  angularDist = vertex_dot(beamPos, beam->dir);

  float parallelism = angularDist / linearDist;
  if(parallelism >= 0) return false;

  return linearDist - radius <= angularDist * parallelism;
}
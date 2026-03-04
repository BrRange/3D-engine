#ifndef COLLIDER_H_
#define COLLIDER_H_

#include "../fold.h"

typedef enum ColliderType{
  ColliderType_Intangible,
  ColliderType_Sphere,
  ColliderType_Pill,
  ColliderType_Beam
} ColliderType;

typedef struct Collider{
  Object *anchor;
  ColliderType type;
  Vertex offset;
} Collider;

typedef struct Collider_Sphere{
  Collider base;
  float radius;
} Collider_Sphere;

Collider_Sphere collider_newSphere(Object *anchor, Vertex offset, float radius);

typedef struct Collider_Pill{
  Collider base;
  Vertex extension;
  float radius;
} Collider_Pill;

Collider_Pill collider_newPill(Object *anchor, Vertex offset, Vertex extension, float radius);

typedef struct Collider_Beam{
  Collider base;
  Vertex dir;
  float maxDist;
} Collider_Beam;

Collider_Beam collider_newBeam(Object *anchor, Vertex offset, Vertex dir, float maxDist);

bool collider_collide(Collider *a, Collider *b);

bool collider_sphere_sphere(Collider_Sphere *a, Collider_Sphere *b);

bool collider_sphere_pill(Collider_Sphere *sphere, Collider_Pill *pill);

bool collider_sphere_beam(Collider_Sphere *sphere, Collider_Beam *beam);

bool collider_pill_pill(Collider_Pill *a, Collider_Pill *b);

bool collider_pill_beam(Collider_Pill *pill, Collider_Beam *beam);

bool collider_beam_beam(Collider_Beam *a, Collider_Beam *b);

#endif
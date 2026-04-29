#ifndef COLLIDER_H_
#define COLLIDER_H_

#include "entity/object.h"

typedef enum ColliderType{
  ColliderType_Intangible,
  ColliderType_Sphere,
  ColliderType_Pill,
  ColliderType_Beam,
  ColliderType_Box
} ColliderType;

typedef enum CollisionType{
  CollisionType_Phase, // ignore
  CollisionType_Slide, // logic in main, multiply by coef to add drag
  CollisionType_Bounce, // add normal with movement mag times coef
  CollisionType_Sink // same as bounce but not correcting the position
} CollisionType;

typedef struct Collider{
  Object *anchor;
  Vec3 offset;
  ColliderType type;
  CollisionType collision;
  f32 coef;
} Collider;

typedef struct CollisionInfo{
  Collider *source, *dest;
  Vec3 normal;
  f32 penetration;
} CollisionInfo;

void collider_setResponse(Collider *coll, CollisionType type, f32 coef);

Vec3 collider_getResponse(Collider *act, Collider *pass, CollisionInfo *info);

typedef struct Collider_Sphere{
  Collider base;
  f32 radius;
} Collider_Sphere;

Collider_Sphere collider_newSphere(Object *anchor, Vec3 offset, f32 radius);

typedef struct Collider_Pill{
  Collider base;
  Vec3 extension;
  f32 radius;
} Collider_Pill;

Collider_Pill collider_newPill(Object *anchor, Vec3 offset, Vec3 extension, f32 radius);

typedef struct Collider_Beam{
  Collider base;
  Vec3 dir;
  f32 maxDist;
} Collider_Beam;

Collider_Beam collider_newBeam(Object *anchor, Vec3 offset, Vec3 dir, f32 maxDist);

typedef struct Collider_Box{
  Collider base;
  Vec3 extension;
} Collider_Box;

Collider_Box collider_newBox(Object *anchor, Vec3 offset, Vec3 extension);

/* Dispatch */
bool collider_collide(Collider *a, Collider *b, CollisionInfo *info);

bool collider_sphere_sphere(Collider_Sphere *a, Collider_Sphere *b, CollisionInfo *info);

bool collider_sphere_pill(Collider_Sphere *sphere, Collider_Pill *pill, CollisionInfo *info);

bool collider_sphere_beam(Collider_Sphere *sphere, Collider_Beam *beam, CollisionInfo *info);

bool collider_sphere_box(Collider_Sphere *sphere, Collider_Box *box, CollisionInfo *info);

bool collider_pill_pill(Collider_Pill *a, Collider_Pill *b, CollisionInfo *info);

bool collider_pill_beam(Collider_Pill *pill, Collider_Beam *beam, CollisionInfo *info);

bool collider_pill_box(Collider_Pill *pill, Collider_Box *box, CollisionInfo *info);

bool collider_beam_beam(Collider_Beam *a, Collider_Beam *b, CollisionInfo *info);

bool collider_beam_box(Collider_Beam *beam, Collider_Box *box, CollisionInfo *info);

bool collider_box_box(Collider_Box *a, Collider_Box *b, CollisionInfo *info);

typedef union Collider_Packed{
  Collider collider;
  Collider_Sphere sphere;
  Collider_Pill pill;
  Collider_Beam beam;
  Collider_Box box;
} Collider_Packed;

#endif
#ifndef PLAYER_H_
#define PLAYER_H_

#include "entity/object.h"

typedef struct Player{
  Object body;
  Collider_Packed hitbox;
  Vec4 speed;
} Player;

Player player_new(Object body, Collider_Packed hitbox);

#endif
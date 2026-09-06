#ifndef PLAYER_H_
#define PLAYER_H_

#include "entity/object.h"

typedef struct Player{
  Object *body;
  Vec4 speed;
} Player;

Player player_new(Object *body, Vec4 pos);

void player_tick(Player *pl, f32 dt);

#endif
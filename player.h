#ifndef PLAYER_H_
#define PLAYER_H_

#include "fold.h"

typedef struct Player{
  Object *body;
  Vertex speed;
} Player;

Player player_new(Object *body, Vertex pos);

void player_tick(Player *pl, f32 dt);

#endif
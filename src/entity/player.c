#include <SDL3/SDL_render.h>
#include "entity/player.h"

Player player_new(Object *body, Vec4 pos){
  Player pl = {
    .body = body
  };
  return pl;
}

void player_tick(Player *pl, f32 dt){
  pl->body->pos += pl->speed;
  if(pl->body->pos[1] >= pl->body->scale / 2){
    pl->body->pos[1] = pl->body->scale / 2;
    if(pl->speed[1] > 0) pl->speed[1] *= -0.6f;
  }
  else pl->speed[1] += 9.8f * dt;
}
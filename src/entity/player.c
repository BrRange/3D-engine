#include <SDL3/SDL_render.h>
#include "include/entity/player.h"

Player player_new(Object *body, Vec3 pos){
  Player pl = {
    .body = body
  };
  return pl;
}

void player_tick(Player *pl, f32 dt){
  pl->body->pos = vec3_add(pl->body->pos, pl->speed);
  if(pl->body->pos.y >= pl->body->scale / 2){
    pl->body->pos.y = pl->body->scale / 2;
    if(pl->speed.y > 0) pl->speed.y *= -0.6f;
  }
  else pl->speed.y += 9.8f * dt;
}
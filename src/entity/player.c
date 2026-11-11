#include <SDL3/SDL_render.h>
#include "entity/player.h"

Player player_new(Object body, Collider_Packed hitbox){
  Player pl = {
    .body = body,
    .hitbox = hitbox
  };
  collider_setResponse(&pl.hitbox.collider, CollisionType_Slide, 1.f);
  return pl;
}
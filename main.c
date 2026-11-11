#include <SDL3/SDL.h>
#include "src/dataType/vector.c"
#include "src/dataType/quaternion.c"
#include "src/dataType/gamectrl.c"
#include "src/dataType/uniform.c"
#include "src/dataType/canvas.c"
#include "src/dataType/darray.c"
#include "src/entity/camera.c"
#include "src/entity/object.c"
#include "src/entity/collider.c"
#include "src/entity/player.c"
#include "eventHandler.h"

typedef struct CommonData{
  f32 deltaT;
  Camera *cam;
  KeyboardHandler *keyboardH;
  MouseHandler *mouseH;
  Canvas *canv;
  Player *player;
  Object *objs;
  usz objCount;
  Collider_Packed *colliders;
  usz collCount;
} CommonData;

void tick(SDL_Renderer *rend, CommonData *data){
  Quaternion rot = quat_new(data->deltaT * 5, vec4_new(0, 1, 0));

  object_rotate(data->objs + 1, rot);

  f32 acc = keyboardH_has(data->keyboardH, SDLK_LSHIFT) ? 200.f : 100.f;
  acc *= data->deltaT;

  Vec2 mouseM = mouseH_getMovement(data->mouseH);

  camera_rotate(data->cam, mouseM.x * 0.01f, mouseM.y * 0.01f);
  Vec4 cameraView = camera_viewVec3(data->cam), rotated;

  data->player->body.pos += data->player->speed * data->deltaT;

  cameraView *= 80.f;
  cameraView = data->player->body.pos - cameraView;
  cameraView -= data->cam->pos;
  camera_moveAbs(data->cam, cameraView / 4.f);

  bool canJump = false;
  Vec4 jumpNormal = {0};

  CollisionInfo info;
  for(i32 i = 0; i < data->collCount; ++i){
    if(collider_collide(&data->player->hitbox.collider, &data->colliders[i].collider, &info)){
      if(info.dest == &data->player->hitbox.collider) info.normal = -info.normal;
      if(info.normal[1] > 0.7f){
        canJump = true;
        jumpNormal += info.normal;
      }
      if(info.snap) data->player->body.pos += info.normal * info.penetration;
      data->player->speed = collision_getResponse(&info, data->player->speed, data->deltaT);
    }
  }

  data->player->speed[1] += 500 * data->deltaT;
  if(canJump){
    acc *= 1.5;
    jumpNormal = vec4_normal(jumpNormal);
  }

  cameraView = camera_viewVec3(data->cam);
  rotated = vec4_new(cameraView[0], 0, cameraView[2]);;
  rotated = vec4_normal(rotated);
  if(keyboardH_has(data->keyboardH, SDLK_W)) data->player->speed += rotated * acc;
  if(keyboardH_has(data->keyboardH, SDLK_S)) data->player->speed -= rotated * acc;
  rotated = vec4_new(-rotated[2], 0, rotated[0]);
  if(keyboardH_has(data->keyboardH, SDLK_A)) data->player->speed += rotated * acc;
  if(keyboardH_has(data->keyboardH, SDLK_D)) data->player->speed -= rotated * acc;
  if(keyboardH_has(data->keyboardH, SDLK_SPACE) && canJump) data->player->speed -= jumpNormal * 250; 

  data->player->speed *= vec4_expand(1 - 0.999 * data->deltaT);
  if(data->player->body.pos[1] > 50) data->player->body.pos = vec4_expand(0);
}

void render(SDL_Renderer *rend, CommonData *data){
  SDL_SetRenderDrawColor(rend, 55, 198, 255, 255);
  SDL_RenderClear(rend);
  Canvas *canv = data->canv;

  canvas_clear(canv);

  object_render(&data->player->body, canv, data->cam);

  for(usz i = 0; i < data->objCount; ++i)
    object_render(data->objs + i, canv, data->cam);

  canvas_render(canv, rend);

  static f32 avgDt = 0.f;

  avgDt += (data->deltaT - avgDt) * data->deltaT;

  char stroing[20];
  SDL_snprintf(stroing, arrLen(stroing), "%02.2f fps", 1.f / avgDt);

  SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
  SDL_RenderDebugText(rend, 1, 1, stroing);
  SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
  SDL_RenderDebugText(rend, 0, 0, stroing);

  SDL_RenderPresent(rend);
}

int main(){
  SDL_Window *win;
  SDL_Renderer *rend;

  SDL_CreateWindowAndRenderer("3D graphics", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED, &win, &rend);

  SDL_SetWindowRelativeMouseMode(win, true);

  SDL_Time start, end = 0, dtime;
  start = SDL_GetTicks();

  KeyboardHandler kbHandler = {0};
  MouseHandler moHandler = {0};

  Canvas canv = canvas_new(rend, 720, 720);
  Camera cam = camera_new(vec4_new(0, 0, 0), 1000.f, .1f, 120.f * SDL_PI_F / 180.f);
  uni_calcPerspective(&cam, 720 /(f32) 1280);

  SDL_Surface *surf = SDL_LoadSurface("res/tex.png");

  Object objs[] = {
    object_new(model_loadFile("res/plane.bbm"), surf, vec4_new(0, 0, 0), 500.f),
    object_new(model_loadFile("res/pylon.bbm"), surf, vec4_new(0, -5, 0), 10.f)
  };

  Player play = player_new(
    object_new(model_loadFile("res/ico.bbm"), surf, vec4_new(0, -10, 0), 10.f),
    (Collider_Packed){.sphere = collider_newSphere(&play.body, vec4_expand(0), 10.f)}
  );

  if(!objs[0].UVmap) SDL_Log(__FILE_NAME__ ":%u %s", __LINE__, SDL_GetError());

  const u32 objLen = arrLen(objs);

  SDL_GetCurrentTime(&dtime);

  Collider_Packed packed[] = {
    [0].box = collider_newBox(
      objs + 0, vec4_new(0, 2, 0), vec4_new(objs[0].scale, 2, objs[0].scale), vec4_new(0, 1, 0)
    ),
    [1].pill = collider_newPill(
      objs + 1, vec4_new(-4 * objs[1].scale, 0, 0), vec4_new(4 * objs[1].scale, 0, 0), objs[1].scale
    )
  };
  collider_setResponse(&packed[0].collider, CollisionType_Slide, 1.f);
  collider_setResponse(&packed[1].collider, CollisionType_Slide, 0.f);

  CommonData data = {
    .cam = &cam,
    .keyboardH = &kbHandler,
    .mouseH = &moHandler,
    .player = &play,
    .objs = objs,
    .canv = &canv,
    .colliders = packed,
    .collCount = arrLen(packed),
    .objCount = objLen,
  };

  for(;;){
    dtime = start - end;
    if(1 || dtime >= 16){
      end = start;
      data.deltaT = dtime / 1000.f;
      uniform.timer += data.deltaT;
      if(!handleEvents(&kbHandler, &moHandler)) break;

      tick(rend, &data);
      render(rend, &data);
    }
    start = SDL_GetTicks();
    SDL_Delay(1);
  }

  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);
  canvas_destroy(&canv);
  SDL_Quit();
}
#include <SDL3/SDL.h>
#include "fold.c"
#include "player.c"
#include "gamectrl.c"
#include "Models/crystalModel.c"
#include "Models/cubeModel.c"
#include "Models/icosphereModel.c"
#include "dataTypes/collider.c"
#include "dataTypes/vector.c"
#include "dataTypes/quaternion.c"
#include "eventHandler.h"
#include "stdfcolor.h"

#define TIMEZONE -3

typedef struct CommonData{
  SDL_Time *deltaT;
  Camera *cam;
  KeyboardHandler *keyboardH;
  MouseHandler *mouseH;
  Canvas *canv;
  Object *objs;
  usz objCount;
  Collider *sphere, *pill;
} CommonData;

void tick(SDL_Renderer *rend, CommonData *data){
  f32 dt = *data->deltaT / 1000.f;

  f32 speed = keyboardH_has(data->keyboardH, SDLK_LSHIFT) ? dt * 600.f : dt * 60.f;

  Object *player = data->objs + 6;

  Quaternion time = quat_new(dt * M_PI / 30.f, vec3_new(0, 0, 1));
  object_rotate(data->objs, time);
  time = quat_new(dt * M_PI / 30.f / 60.f, vec3_new(0, 0, 1));
  object_rotate(data->objs + 1, time);
  time = quat_new(dt * M_PI / 30.f / 60.f / 12.f, vec3_new(0, 0, 1));
  object_rotate(data->objs + 2, time);

  Vec2 mouseM = mouseH_getMovement(data->mouseH);

  camera_rotate(data->cam, mouseM.x * 0.01f, mouseM.y * 0.01f);
  Vec3 cameraView = camera_viewVec3(data->cam), rotated;

  if(keyboardH_has(data->keyboardH, SDLK_W)) object_move(player, vec3_mul(cameraView, speed));
  if(keyboardH_has(data->keyboardH, SDLK_S)) object_move(player, vec3_mul(cameraView, -speed));
  rotated = vec3_new(-cameraView.z, 0, cameraView.x);
  rotated = vec3_div(rotated, vec3_mag(rotated));
  if(keyboardH_has(data->keyboardH, SDLK_A)) object_move(player, vec3_mul(rotated, speed));
  if(keyboardH_has(data->keyboardH, SDLK_D)) object_move(player, vec3_mul(rotated, -speed));
  if(keyboardH_has(data->keyboardH, SDLK_SPACE)) object_move(player, vec3_new(0, -speed, 0));
  if(keyboardH_has(data->keyboardH, SDLK_LCTRL)) object_move(player, vec3_new(0, speed, 0));

  cameraView = vec3_mul(cameraView, 100);
  cameraView = vec3_sub(player->pos, cameraView);
  cameraView = vec3_sub(cameraView, data->cam->pos);
  camera_moveAbs(data->cam, vec3_div(cameraView, 4));

  if(collider_collide(data->sphere, data->pill)) data->objs[6].scale = 10;
  else data->objs[6].scale = 5;
}

void render(SDL_Renderer *rend, CommonData *data){
  SDL_SetRenderDrawColor(rend, 55, 198, 255, 255);
  SDL_RenderClear(rend);
  Canvas *canv = data->canv;

  canvas_clear(canv);

  for(usz i = 0; i < data->objCount; ++i)
    object_render(data->objs + i, canv, data->cam);

  canvas_render(canv, rend);

  SDL_RenderPresent(rend);
}

int main(){
  SDL_Window *win;
  SDL_Renderer *rend;

  SDL_CreateWindowAndRenderer("3D graphics", 1280, 720, 0, &win, &rend);

  SDL_SetWindowRelativeMouseMode(win, true);

  SDL_Time start, end = 0, dtime;
  start = SDL_GetTicks();

  KeyboardHandler kbHandler = {0};
  MouseHandler moHandler = {0};
  Canvas canv = canvas_new(rend, 1280, 720);

  Camera cam = camera(vec3_new(0, 0, 0), 200, 1.f, 1.f / SDL_tanf(2.f * SDL_PI_F / 3.f / 2.f));

  Color colors[] = {
    Color_Red,    // 0
    Color_Green,  // 1
    Color_Blue,   // 2
    Color_White,  // 3
    Color_Yellow, // 4
    Color_Cyan,   // 5
    Color_Magenta, // 6
    Color_Black // 7
  };

  Vec3 zeCube_vert[] = {
    vec3_new(1, -1, -1),
    vec3_new(-1, -1, -1),
    vec3_new(-1, -1, 1),
    vec3_new(1, -1, 1),
    vec3_new(1, 1, -1),
    vec3_new(-1, 1, -1),
    vec3_new(-1, 1, 1),
    vec3_new(1, 1, 1)
  };

  Polygon zeCube_poly[] = {
    polygon_new(2, 1, 0, 0),
    polygon_new(3, 2, 0, 0),
    polygon_new(1, 4, 0, 0),
    polygon_new(5, 4, 1, 0),
    polygon_new(4, 3, 0, 0),
    polygon_new(4, 7, 3, 0),
    polygon_new(2, 5, 1, 0),
    polygon_new(6, 5, 2, 0),
    polygon_new(3, 6, 2, 0),
    polygon_new(7, 6, 3, 0),
    polygon_new(5, 6, 4, 0),
    polygon_new(6, 7, 4, 0)
  };

  Model zeCube_model = model(zeCube_vert, arrLen(zeCube_vert), zeCube_poly, arrLen(zeCube_poly));

  Vec3 vert[][4] = {
    {vec3_new(.01, -1, 0), vec3_new(-.01, -1, 0), vec3_new(-.01, 0, 0), vec3_new(.01, 0, 0)},
    {vec3_new(.02, -1, 0), vec3_new(-.02, -1, 0), vec3_new(-.02, 0, 0), vec3_new(.02, 0, 0)},
    {vec3_new(.05, -1, 0), vec3_new(-.05, -1, 0), vec3_new(-.05, 0, 0), vec3_new(.05, 0, 0)},
  };
  Polygon poly[] = {polygon_new(0, 1, 2, 0), polygon_new(0, 2, 3, 0)};
  Model models[] = {
    {.vec3 = vert[0], .vec3Count = arrLen(vert[0]), .polygon = poly, .polyCount = arrLen(poly)},
    {.vec3 = vert[1], .vec3Count = arrLen(vert[1]), .polygon = poly, .polyCount = arrLen(poly)},
    {.vec3 = vert[2], .vec3Count = arrLen(vert[2]), .polygon = poly, .polyCount = arrLen(poly)}
  };

  Vec3 backdropVert[] = {
    vec3_new(0.5f, -sqrtf(3) / 2.f, 0),
    vec3_new(-0.5f, -sqrtf(3) / 2.f, 0),
    vec3_new(-1, 0, 0),
    vec3_new(-0.5f, sqrtf(3) / 2.f, 0),
    vec3_new(0.5f, sqrtf(3) / 2.f, 0),
    vec3_new(1, 0, 0)
  };
  Polygon backdropPoly[] = {
    polygon_new(0, 1, 5, 0),
    polygon_new(1, 2, 5, 0),
    polygon_new(2, 3, 4, 0),
    polygon_new(2, 4, 5, 0)
  };
  Model backdropModel = {.vec3 = backdropVert, .vec3Count = arrLen(backdropVert), .polygon = backdropPoly, .polyCount = arrLen(backdropPoly)};

  Vec3 thick_vert[] = {
    vec3_new(0.5f, -sqrtf(3) / 2.f, 0),
    vec3_new(-0.5f, -sqrtf(3) / 2.f, 0),
    vec3_new(-1, 0, 0),
    vec3_new(-0.5f, sqrtf(3) / 2.f, 0),
    vec3_new(0.5f, sqrtf(3) / 2.f, 0),
    vec3_new(1, 0, 0),
    vec3_new(0.5f, -sqrtf(3) / 2.f, 0.1),
    vec3_new(-0.5f, -sqrtf(3) / 2.f, 0.1),
    vec3_new(-1, 0, 0.1),
    vec3_new(-0.5f, sqrtf(3) / 2.f, 0.1),
    vec3_new(0.5f, sqrtf(3) / 2.f, 0.1),
    vec3_new(1, 0, 0.1)
  };

  Polygon thick_poly[] = {
    polygon_new(0, 1, 5, 0),
    polygon_new(1, 2, 5, 0),
    polygon_new(2, 3, 4, 0),
    polygon_new(2, 4, 5, 0),
    polygon_new(6, 7, 11, 0),
    polygon_new(7, 8, 11, 0),
    polygon_new(8, 9, 10, 0),
    polygon_new(8, 10, 11, 0),
    polygon_new(0, 6, 1, 0),
    polygon_new(7, 6, 1, 0),
    polygon_new(1, 7, 2, 0),
    polygon_new(8, 7, 2, 0),
    polygon_new(2, 8, 3, 0),
    polygon_new(9, 8, 3, 0),
    polygon_new(3, 9, 4, 0),
    polygon_new(10, 9, 4, 0),
    polygon_new(4, 10, 5, 0),
    polygon_new(11, 10, 5, 0),
    polygon_new(5, 11, 6, 0),
    polygon_new(5, 0, 6, 0)
  };

  Model thick_model = model(thick_vert, arrLen(thick_vert), thick_poly, arrLen(thick_poly));

  #define zeDim 10

  Object objs[(zeDim * zeDim) + 7] = {
    object_new(&models[0], colors + 7, vec3_new(-20, 5, 19.8), 100),
    object_new(&models[1], colors + 7, vec3_new(-20, 5, 19.8), 90),
    object_new(&models[2], colors + 7, vec3_new(-20, 5, 19.8), 40),
    object_new(&backdropModel, colors + 7, vec3_new(-20, 5, 19.8), 5),
    object_new(&backdropModel, colors + 3, vec3_new(-20, 5, 19.9), 120),
    object_new(&thick_model, colors + 2, vec3_new(-20, 5, 20), 130),
    object_new(&zeCube_model, colors, vec3_new(0, 0, 0), 5)
  };

  for(i32 i = 0; i < zeDim; ++i)
  for(i32 j = 0; j < zeDim; ++j){
    Object *objIns = objs + 7 + zeDim * i + j;
    *objIns = object_new(&zeCube_model, colors + ((i + j) % 8), vec3_new(i * 40, j * 40, 500), 20);
  }

  const u32 objLen = arrLen(objs);

  u32 renderList[objLen];
  for(u32 u = 0; u < objLen; ++u) renderList[u] = u;

  SDL_GetCurrentTime(&dtime);
  float clockAngle = dtime / 1e9f;
  Quaternion time = quat_new(clockAngle * M_PI / 30.f, vec3_new(0, 0, 1));
  object_rotate(objs + 0, time);
  clockAngle /= 60.f;
  time = quat_new(clockAngle * M_PI / 30.f, vec3_new(0, 0, 1));
  object_rotate(objs + 1, time);
  clockAngle /= 60.f;
  clockAngle += TIMEZONE;
  time = quat_new(clockAngle * M_PI / 6.f, vec3_new(0, 0, 1));
  object_rotate(objs + 2, time);

  Collider_Sphere playerColl = collider_newSphere(objs + 6, vec3_new(0, 0, 0), objs[6].scale);
  Collider_Pill secondColl = collider_newPill(objs + 3, vec3_new(0, -100, 0), vec3_new(0, 100, 0), 5);

  CommonData data = {
    .deltaT = &dtime,
    .cam = &cam,
    .keyboardH = &kbHandler,
    .mouseH = &moHandler,
    .objs = objs,
    .canv = &canv,
    .objCount = objLen,
    .sphere = (Collider*)&playerColl,
    .pill = (Collider*)&secondColl
  };

  bool running = true;

 for(;;){
    dtime = start - end;
    if(dtime >= 16){
      end = start;
      if(!handleEvents(&kbHandler, &moHandler)) break;;

      tick(rend, &data);
      render(rend, &data);
    }
    start = SDL_GetTicks();
    SDL_Delay(1);
  }

  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);
  canvas_destroy(&canv);
  SDL_assert((SDL_Log("Program in debug mode"), 0));
  SDL_Quit();
}
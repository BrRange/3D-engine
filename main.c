#include <SDL3/SDL.h>
#include "src/dataType/vector.c"
#include "src/dataType/quaternion.c"
#include "src/dataType/gamectrl.c"
#include "src/dataType/canvas.c"
#include "src/entity/camera.c"
#include "src/entity/object.c"
#include "src/entity/collider.c"
#include "src/entity/player.c"
#include "src/entity/lightSource.c"
#include "eventHandler.h"

typedef struct CommonData{
  SDL_Time *deltaT;
  Camera *cam;
  KeyboardHandler *keyboardH;
  MouseHandler *mouseH;
  Canvas *canv;
  Object *objs;
  usz objCount;
  Collider_Packed *activeColl, *passiveColl;
  usz activeCollLen, passiveCollLen;
  LightSource_Packed sources[2];
} CommonData;

void tick(SDL_Renderer *rend, CommonData *data){
  static bool ableJump = false;
  static f32 vertical = 0.f;
  f32 dt = *data->deltaT / 1000.f;

  f32 speed = keyboardH_has(data->keyboardH, SDLK_LSHIFT) ? dt * 120.f : dt * 60.f;

  Object *player = data->objs + 6;

  Quaternion time = quat_new(dt * M_PI / 30.f, vec3_new(0, 0, 1));
  object_rotate(data->objs, time);
  time = quat_new(dt * M_PI / 30.f / 60.f, vec3_new(0, 0, 1));
  object_rotate(data->objs + 1, time);
  time = quat_new(dt * M_PI / 30.f / 60.f / 12.f, vec3_new(0, 0, 1));
  object_rotate(data->objs + 2, time);

  time = quat_new(dt, vec3_new(0, 1, 0));
  object_rotate(data->objs + 7, time);
  object_rotate(data->objs + 8, time);

  Vec2 mouseM = mouseH_getMovement(data->mouseH);

  camera_rotate(data->cam, mouseM.x * 0.01f, mouseM.y * 0.01f);
  Vec3 cameraView = camera_viewVec3(data->cam), rotated;
  Vec3 noFlying = cameraView;
  noFlying.y = 0;

  if(keyboardH_has(data->keyboardH, SDLK_W)) object_move(player, vec3_mul(noFlying, speed));
  if(keyboardH_has(data->keyboardH, SDLK_S)) object_move(player, vec3_mul(noFlying, -speed));
  rotated = vec3_new(-cameraView.z, 0, cameraView.x);
  rotated = vec3_div(rotated, vec3_mag(rotated));
  if(keyboardH_has(data->keyboardH, SDLK_A)) object_move(player, vec3_mul(rotated, speed));
  if(keyboardH_has(data->keyboardH, SDLK_D)) object_move(player, vec3_mul(rotated, -speed));
  //if(keyboardH_has(data->keyboardH, SDLK_SPACE)) object_move(player, vec3_new(0, -speed, 0));
  if(keyboardH_has(data->keyboardH, SDLK_SPACE) && ableJump){
    vertical = -3.f;
    ableJump = false;
  }
  //if(keyboardH_has(data->keyboardH, SDLK_LCTRL)) object_move(player, vec3_new(0, speed, 0));

  cameraView = vec3_mul(cameraView, 80);
  cameraView = vec3_sub(player->pos, cameraView);
  cameraView = vec3_sub(cameraView, data->cam->pos);
  camera_moveAbs(data->cam, vec3_div(cameraView, 4));

  player->pos = vec3_add(player->pos, vec3_new(0, vertical, 0));
  if(vertical < 2.f) vertical += 0.2f;

  CollisionInfo cinfo;

  for(usz i = 0; i < data->passiveCollLen; ++i){
    Collider *act = &data->activeColl->collider, *pass = &(data->passiveColl + i)->collider;
    if(collider_collide(act, pass, &cinfo)){
      if(cinfo.normal.y > 0.9f) ableJump = true;
      else ableJump = false;
      f32 invert = cinfo.source == act ? 1 : -1;
      cinfo.normal = vec3_mul(cinfo.normal, cinfo.penetration * invert);
      player->pos = vec3_add(player->pos, cinfo.normal);
    }
  }
}

void render(SDL_Renderer *rend, CommonData *data){
  SDL_SetRenderDrawColor(rend, 55, 198, 255, 255);
  SDL_RenderClear(rend);
  Canvas *canv = data->canv;

  canvas_clear(canv);

  for(usz i = 0; i < data->objCount; ++i)
    object_render(data->objs + i, canv, data->cam, data->sources);

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

  Camera cam = camera_new(vec3_new(0, 0, 0), 200, 1.f, 1.f / SDL_tanf(2.f * SDL_PI_F / 3.f / 2.f));

  Color colors[] = {
    color_new(1, 0, 0),
    color_new(0, 1, 0),
    color_new(0, 0, 1),
    color_new(1, 1, 1),
    color_new(0, 1, 1),
    color_new(1, 0, 1),
    color_new(1, 1, 0),
    color_new(0, 0, 0)
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

  Polygon zeInv_poly[] = {
    polygon_new(2, 0, 1, 0),
    polygon_new(3, 0, 2, 0),
    polygon_new(1, 0, 4, 0),
    polygon_new(5, 1, 4, 0),
    polygon_new(4, 0, 3, 0),
    polygon_new(4, 3, 7, 0),
    polygon_new(2, 1, 5, 0),
    polygon_new(6, 2, 5, 0),
    polygon_new(3, 2, 6, 0),
    polygon_new(7, 3, 6, 0),
    polygon_new(5, 4, 6, 0),
    polygon_new(6, 4, 7, 0)
  };

  Model zeInv_model = model(zeCube_vert, arrLen(zeCube_vert), zeInv_poly, arrLen(zeInv_poly));

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
    polygon_new(11, 7, 6, 0),
    polygon_new(11, 8, 7, 0),
    polygon_new(10, 9, 8, 0),
    polygon_new(11, 10, 8, 0),
    polygon_new(0, 6, 1, 0),
    polygon_new(1, 6, 7, 0),
    polygon_new(1, 7, 2, 0),
    polygon_new(2, 7, 8, 0),
    polygon_new(2, 8, 3, 0),
    polygon_new(3, 8, 9, 0),
    polygon_new(3, 9, 4, 0),
    polygon_new(4, 9, 10, 0),
    polygon_new(4, 10, 5, 0),
    polygon_new(5, 10, 11, 0),
    polygon_new(5, 11, 6, 0),
    polygon_new(6, 0, 5, 0)
  };

  Model thick_model = model(thick_vert, arrLen(thick_vert), thick_poly, arrLen(thick_poly));

  Vec3 plane_vert[] = {
    vec3_new(-1, 0, 1),
    vec3_new(-1, 0, -1),
    vec3_new(1, 0, -1),
    vec3_new(1, 0, 1)
  };

  Polygon plane_poly[] = {
    polygon_new(0, 1, 2, 0),
    polygon_new(0, 2, 3, 0)
  };

  Model plane_model = model(plane_vert, arrLen(plane_vert), plane_poly, arrLen(plane_poly));

  Object objs[] = {
    object_new(&models[0], colors + 7, vec3_new(-20, 5 - 100, 19.8), 100),
    object_new(&models[1], colors + 7, vec3_new(-20, 5 - 100, 19.8), 90),
    object_new(&models[2], colors + 7, vec3_new(-20, 5 - 100, 19.8), 40),
    object_new(&backdropModel, colors + 7, vec3_new(-20, 5 - 100, 19.8), 5),
    object_new(&backdropModel, colors + 3, vec3_new(-20, 5 - 100, 19.9), 120),
    object_new(&thick_model, colors + 2, vec3_new(-20, 5 - 100, 20), 130),
    object_new(&zeCube_model, colors, vec3_expand(0), 5),
    object_new(&zeCube_model, colors + 4, vec3_new(0, 0, -200), 10),
    object_new(&zeInv_model, colors + 7, vec3_new(0, 0, -200), 11),
    object_new(&plane_model, colors + 2, vec3_new(0, 15, 0), 1000)
  };

  const u32 objLen = arrLen(objs);

  u32 renderList[objLen];
  for(u32 u = 0; u < objLen; ++u) renderList[u] = u;

  SDL_GetCurrentTime(&dtime);
  SDL_DateTime dateTime;
  SDL_TimeToDateTime(dtime, &dateTime, true);

  Quaternion time = quat_new(dateTime.second * M_PI / 30.f, vec3_new(0, 0, 1));
  object_rotate(objs + 0, time);
  time = quat_new(dateTime.minute * M_PI / 30.f, vec3_new(0, 0, 1));
  object_rotate(objs + 1, time);
  time = quat_new(dateTime.hour * M_PI / 6.f, vec3_new(0, 0, 1));
  object_rotate(objs + 2, time);

  Collider_Packed active[1] = {{ .sphere = collider_newSphere(objs + 6, vec3_new(0, 0, 0), objs[6].scale) }};
  Collider_Packed passive[2] = {
    {.box = collider_newBox(objs + 7, vec3_expand(0), vec3_expand(objs[7].scale))},
    {.box = collider_newBox(objs + 9, vec3_expand(0), vec3_new(objs[9].scale, 0, objs[9].scale))}
  };

  CommonData data = {
    .deltaT = &dtime,
    .cam = &cam,
    .keyboardH = &kbHandler,
    .mouseH = &moHandler,
    .objs = objs,
    .canv = &canv,
    .objCount = objLen,
    .activeColl = active,
    .activeCollLen = arrLen(active),
    .passiveColl = passive,
    .passiveCollLen = arrLen(passive),
    .sources = {
      {.diffuse = lightSource_newDiffuse(vec3_new(1.4, 1.4, 0.8), vec3_new(1, -1, -1))},
      {.ambient = lightSource_newAmbient(vec3_new(0.4, 0.6, 0.4))}
    }
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
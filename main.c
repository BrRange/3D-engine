#include <SDL3/SDL.h>
#include "src/dataType/vector.c"
#include "src/dataType/quaternion.c"
#include "src/dataType/gamectrl.c"
#include "src/dataType/uniform.c"
#include "src/dataType/canvas.c"
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
  Object *objs;
  usz objCount;
} CommonData;

void tick(SDL_Renderer *rend, CommonData *data){
  static Vec4 pSpeed;

  Quaternion rot = quat_new(data->deltaT, vec4_new(0, 1, 0));
  //object_rotate(data->objs, rot);

  f32 acc = keyboardH_has(data->keyboardH, SDLK_LSHIFT) ? 1.2f : .5f;

  Vec2 mouseM = mouseH_getMovement(data->mouseH);

  camera_rotate(data->cam, mouseM.x * 0.01f, mouseM.y * 0.01f);
  Vec4 cameraView = camera_viewVec3(data->cam), rotated;

  if(keyboardH_has(data->keyboardH, SDLK_SPACE)) camera_moveAbs(data->cam, vec4_new(0, -acc, 0));
  if(keyboardH_has(data->keyboardH, SDLK_LCTRL)) camera_moveAbs(data->cam, vec4_new(0, acc, 0));

  cameraView = camera_viewVec3(data->cam);
  rotated = cameraView;
  if(keyboardH_has(data->keyboardH, SDLK_W)) camera_moveAbs(data->cam, rotated * acc);
  if(keyboardH_has(data->keyboardH, SDLK_S)) camera_moveAbs(data->cam, rotated * -acc);
  rotated = vec4_new(-cameraView[2], 0, cameraView[0]);
  rotated = vec4_normal(rotated);
  if(keyboardH_has(data->keyboardH, SDLK_A)) camera_moveAbs(data->cam, rotated * acc);
  if(keyboardH_has(data->keyboardH, SDLK_D)) camera_moveAbs(data->cam, rotated * -acc);
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

void build_cube(Vec4 *vert, Polygon *poly){
  for(int i = 0; i < 8; ++i)
  vert[i] = vec4_new((i & 1) ? 1 : -1, (i & 2) ? 1 : -1, (i & 4) ? 1 : -1);

  #define oth (1.f/3.f)
  #define tth (2.f/3.f)

  u16 idx[3];
  Vec2 uv[3];

  idx[0] = 0, idx[1] = 2, idx[2] = 3;
  uv[0] = vec2_new(0.25, oth), uv[1] = vec2_new(0.25, tth), uv[2] = vec2_new(0.5, tth);
  poly[0] = polygon_new(vec4_new(0, 0, -1), uv, idx);
  idx[1] = idx[2], idx[2] = 1;
  uv[1] = uv[2], uv[2] = vec2_new(0.5, oth);
  poly[1] = polygon_new(vec4_new(0, 0, -1), uv, idx);
  
  idx[0] = 4, idx[1] = 0, idx[2] = 1;
  uv[0] = vec2_new(0.25, 0), uv[1] = vec2_new(0.25, oth), uv[2] = vec2_new(0.5, oth);
  poly[2] = polygon_new(vec4_new(0, -1, 0), uv, idx);
  idx[1] = idx[2], idx[2] = 5;
  uv[1] = uv[2], uv[2] = vec2_new(0.5, 0);
  poly[3] = polygon_new(vec4_new(0, -1, 0), uv, idx);

  idx[0] = 4, idx[1] = 6, idx[2] = 2;
  uv[0] = vec2_new(0, oth), uv[1] = vec2_new(0, tth), uv[2] = vec2_new(0.25, tth);
  poly[4] = polygon_new(vec4_new(0, -1, 0), uv, idx);
  idx[1] = idx[2], idx[2] = 0;
  uv[1] = uv[2], uv[2] = vec2_new(0.25, oth);
  poly[5] = polygon_new(vec4_new(0, -1, 0), uv, idx);

  idx[0] = 1, idx[1] = 3, idx[2] = 7;
  uv[0] = vec2_new(0.5, oth), uv[1] = vec2_new(0.5, tth), uv[2] = vec2_new(0.75, tth);
  poly[6] = polygon_new(vec4_new(0, -1, 0), uv, idx);
  idx[1] = idx[2], idx[2] = 5;
  uv[1] = uv[2], uv[2] = vec2_new(0.75, oth);
  poly[7] = polygon_new(vec4_new(0, -1, 0), uv, idx);

  idx[0] = 2, idx[1] = 6, idx[2] = 7;
  uv[0] = vec2_new(0.25, tth), uv[1] = vec2_new(0.25, 1), uv[2] = vec2_new(0.5, 1);
  poly[8] = polygon_new(vec4_new(0, -1, 0), uv, idx);
  idx[1] = idx[2], idx[2] = 3;
  uv[1] = uv[2], uv[2] = vec2_new(0.5, tth);
  poly[9] = polygon_new(vec4_new(0, -1, 0), uv, idx);

  idx[0] = 5, idx[1] = 7, idx[2] = 6;
  uv[0] = vec2_new(0.75, oth), uv[1] = vec2_new(0.75, tth), uv[2] = vec2_new(1, tth);
  poly[10] = polygon_new(vec4_new(0, -1, 0), uv, idx);
  idx[1] = idx[2], idx[2] = 4;
  uv[1] = uv[2], uv[2] = vec2_new(1, oth);
  poly[11] = polygon_new(vec4_new(0, -1, 0), uv, idx);
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

  Canvas canv = canvas_new(rend, 800, 800);
  Camera cam = camera_new(vec4_new(0, 0, 0), 1000.f, .1f, 120.f * SDL_PI_F / 180.f);
  uni_calcPerspective(&cam, 720 /(f32) 1280);

  Vec4 vert[8];
  Polygon poly[12];

  build_cube(vert, poly);

  Model mod = model_new(vert, poly, arrLen(poly));

  Object objs[] = {object_new(&mod, SDL_LoadSurface("tex.png"), vec4_new(0, 0.2, 1), .2f)};

  if(!objs[0].UVmap) SDL_Log(__FILE_NAME__ ":%u %s", __LINE__, SDL_GetError());

  const u32 objLen = arrLen(objs);

  SDL_GetCurrentTime(&dtime);

  CommonData data = {
    .cam = &cam,
    .keyboardH = &kbHandler,
    .mouseH = &moHandler,
    .objs = objs,
    .canv = &canv,
    .objCount = objLen,
  };
  bool running = true;

 for(;;){
    dtime = start - end;
    if(dtime >= 16){
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
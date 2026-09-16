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
  Object *objs;
  usz objCount;
} CommonData;

void tick(SDL_Renderer *rend, CommonData *data){
  static Vec4 pSpeed;

  Quaternion rot = quat_new(data->deltaT * .5, vec4_new(0, 1, 0));
  for(i32 i = 0; i < data->objCount; ++i)
  object_rotate(data->objs + i, rot);

  f32 acc = keyboardH_has(data->keyboardH, SDLK_LSHIFT) ? .05f : .01f;

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

  Canvas canv = canvas_new(rend, 800, 800);
  Camera cam = camera_new(vec4_new(0, 0, 0), 1000.f, .1f, 120.f * SDL_PI_F / 180.f);
  uni_calcPerspective(&cam, 720 /(f32) 1280);

  Object objs[] = {
    object_new(model_loadFile("model.bbm"), SDL_LoadSurface("tex.png"), vec4_new(0, 0.2, 1), 1.f)
  };

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
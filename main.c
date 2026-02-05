#include <SDL3/SDL.h>
#include "fold.c"
#include "player.c"
#include "sdlFrame.c"
#include "Models/crystalModel.c"
#include "Models/cubeModel.c"
#include "Models/icosphereModel.c"
#include "eventHandler.h"
#include "stdfcolor.h"

typedef struct CommonData{
  SDL_Time *deltaT;
  Camera *cam;
  MenuState *state;
  Object *objs;
  u32 *renderList;
  usz objCount;
} CommonData;

void tick(SDL_Renderer *rend, CommonData *data){
  f32 dt = *data->deltaT / 1000.f;

  f32 speed = KeyboardHandler_hasKey(data->state->keyboardH, SDLK_LSHIFT) ? dt * 600.f : dt * 60.f;
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_LALT)) speed *= 100000.f;

  SDL_FPoint mouseM = MouseHandler_getMovement(data->state->mouseH);

  camera_rotate(data->cam, mouseM.x * 0.01f, mouseM.y * 0.01f);
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_W)) camera_moveRel(data->cam, vertex_new(0, 0, speed));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_S)) camera_moveRel(data->cam, vertex_new(0, 0, -speed));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_A)) camera_moveRel(data->cam, vertex_new(-speed, 0, 0));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_D)) camera_moveRel(data->cam, vertex_new(speed, 0, 0));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_SPACE)) camera_moveRel(data->cam, vertex_new(0, -speed, 0));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_LCTRL)) camera_moveRel(data->cam, vertex_new(0, speed, 0));

  void *sortData[2] = {data->cam, data->objs};

  SDL_qsort_r(data->renderList, data->objCount, sizeof *data->renderList, (void*)object_distCompare, sortData);
}

void render(SDL_Renderer *rend, CommonData *data){
  int w, h;
  SDL_GetRenderOutputSize(rend, &w, &h);
  f32 cx = w / 2, cy = h / 2;
  SDL_SetRenderDrawColor(rend, 55, 198, 255, 255);
  SDL_RenderClear(rend);

  for(int i = 0; i < data->objCount; ++i)
  object_render(data->objs + data->renderList[i], rend, data->cam, cx, cy);
}

int main(){
  SDL_Window *win;
  SDL_Renderer *rend;

  SDL_CreateWindowAndRenderer("3D graphics", 400, 400, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED, &win, &rend);

  SDL_SetWindowRelativeMouseMode(win, true);
  SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

  Uint64 start, end = 0, dtime;
  start = SDL_GetTicks();

  KeyboardHandler kbHandler = {0};
  MouseHandler moHandler = {0};
  MenuState menu = MenuState_new(NULL, NULL, &kbHandler, &moHandler, 0);
  Camera cam = camera(vertex_new(0, 0, 0), 200, 0.01f, 500);

  Color colors[] = {
    Color_Red,    //0
    Color_Green,  //1
    Color_Blue,   //2
    Color_White,  //3
    Color_Yellow, //4
    Color_Cyan,   //5
    Color_Magenta //6
  };

  Vertex cube_vert[8], crystal_vert[6], sun_vert[12];
  Polygon cube_poly[12], crystal_poly[8], sun_poly[20];
  Model cube_model = cube_gen(cube_vert, cube_poly), crystal_model = crystal_gen(crystal_vert, crystal_poly, 0.25f), sun_model = icosphere_gen(sun_vert, sun_poly, 0);

  Object objs[] = {
    object_new(&cube_model, colors+0, vertex_new(-20, 5, 20), 10),
    object_new(&cube_model, colors+1, vertex_new(-20, 5, 30), 10),
    object_new(&cube_model, colors+2, vertex_new(-20, 5, 40), 10),
    object_new(&cube_model, colors+3, vertex_new(-20, 5, 50), 10),
    object_new(&cube_model, colors+4, vertex_new(-20, 5, 60), 10),
    object_new(&cube_model, colors+5, vertex_new(-20, 5, 70), 10),
    object_new(&cube_model, colors+6, vertex_new(-20, 5, 80), 10),
    object_new(&sun_model, colors+4, vertex_new(1000, -100000000, 1000), 10000000)
  };
  const u32 objLen = arrLen(objs);

  u32 renderList[objLen];
  for(u32 u = 0; u < objLen; ++u) renderList[u] = u;


  CommonData data = {
    .deltaT = &dtime,
    .cam = &cam,
    .state = &menu,
    .objs = objs,
    .renderList = renderList,
    .objCount = objLen,
  };

  SDL_Event eve = {0};
  while(eve.type != SDL_EVENT_QUIT){
    dtime = start - end;
    if(dtime >= 16){
      end = start;
      handleEvents(&eve, &menu);

      tick(rend, &data);
      render(rend, &data);

      SDL_RenderPresent(rend);
    }
    start = SDL_GetTicks();
    SDL_Delay(1);
  }

  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);
  SDL_assert(!"Program was in debug mode");
  SDL_Log("Normal execution");
}
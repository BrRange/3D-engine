#include <SDL3/SDL.h>
#include "fold.c"
#include "arena.c"
#include "sdlFrame.c"
#include "Models/planeModel.c"
#include "Models/cubeModel.c"
#include "eventHandler.h"
#include "stdfcolor.h"

#include <SDL3_gfx/SDL3_gfxPrimitives_font.h>

#define arrLen(_arr) (sizeof(_arr) / sizeof(*(_arr)))

typedef struct CommonData{
  SDL_Time *deltaT;
  Camera *cam;
  MenuState *state;
  Object *objs;
  usz objCount;
} CommonData;

void tick(SDL_Renderer *rend, CommonData *data){

  f32 dt = *data->deltaT / 1000000.f;

  SDL_qsort_r(data->objs, data->objCount, sizeof(Object), (void*)object_distCompare, data->cam);

  f32 speed = KeyboardHandler_hasKey(data->state->keyboardH, SDLK_LSHIFT) ? dt / 6 : dt / 60;

  SDL_FPoint mouseM = MouseHandler_getMovement(data->state->mouseH);

  camera_rotate(data->cam, mouseM.x * 0.01f, mouseM.y * 0.01f);
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_W)) camera_moveRel(data->cam, vertex(0, 0, speed));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_S)) camera_moveRel(data->cam, vertex(0, 0, -speed));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_A)) camera_moveRel(data->cam, vertex(-speed, 0, 0));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_D)) camera_moveRel(data->cam, vertex(speed, 0, 0));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_SPACE)) camera_moveRel(data->cam, vertex(0, -speed, 0));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_LCTRL)) camera_moveRel(data->cam, vertex(0, speed, 0));
}

void render(SDL_Renderer *rend, CommonData *data){
  int w, h;
  SDL_GetRenderOutputSize(rend, &w, &h);
  f32 cx = w / 2, cy = h / 2;
  SDL_RenderClear(rend);

  for(int i = 0; i < data->objCount; ++i)
  object_render(data->objs + i, rend, data->cam, cx, cy);
}

int main(){
  SDL_Window *win;
  SDL_Renderer *rend;

  SDL_CreateWindowAndRenderer("3D graphics", 400, 400, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED, &win, &rend);

  SDL_SetWindowRelativeMouseMode(win, true);
  SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

  SDL_Time start, end = 0, dtime;
  SDL_GetCurrentTime(&start);
  SDL_srand(start);

  Vertex plane_vert[4];
  Polygon plane_poly[2];
  Model plane_model = plane_gen(plane_vert, plane_poly);

  KeyboardHandler kbHandler = {0};
  MouseHandler moHandler = {0};
  MenuState menu = MenuState_new(NULL, NULL, &kbHandler, &moHandler, 0);
  Camera cam = camera(vertex(0, 0, 0), 200, 0.01f, 500);

  Color colors[] = {
    Color_Red,    //0
    Color_Green,  //1
    Color_Blue,   //2
    Color_White,  //3
    Color_Yellow, //4
    Color_Cyan,   //5
    Color_Magenta //6
  };

  Vertex cube_vert[8];
  Polygon cube_poly[12];
  Model cube_model = cube_gen(cube_vert, cube_poly);

  Object objs[] = {
    object(&plane_model, colors+3, vertex(0, 10, 0), 1e6f),
    object(&cube_model, colors+0, vertex(0, 5, 0), 10)
  };
  int objLen = arrLen(objs);

  CommonData data = {&dtime, &cam, &menu, objs, objLen};

  SDL_Event eve = {0};
  while(eve.type != SDL_EVENT_QUIT){
    dtime = start - end;
    if(dtime >= 16000000){
      end = start;
      handleEvents(&eve, &menu);

      tick(rend, &data);
      render(rend, &data);

      SDL_RenderPresent(rend);
    }
    SDL_GetCurrentTime(&start);
    SDL_Delay(1);
  }

  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);
  SDL_assert(!"Program was in debug mode");
  SDL_Log("Normal execution");
}
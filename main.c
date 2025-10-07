#include <SDL3/SDL.h>
#include "fold.c"
#include "arena.c"
#include "sdlFrame.c"
#include "Models/icosphereModel.c"
#include "Models/cubeModel.c"
#include "Models/crystalModel.c"
#include "eventHandler.h"

#define arrLen(_arr) (sizeof(_arr) / sizeof(*(_arr)))
#define boo 50
#define booboo (boo*boo)

void tick(SDL_Renderer *rend, void **args){
  SDL_Time *dt = args[0];
  Camera *cam = args[1];
  MenuState *menu = args[2];
  Object *objs = args[3];
  int *objCount = args[4];

  float deltaT = *dt / 1000000.f;

  object_rotateY(objs, deltaT * 2.f * SDL_PI_F / 2000.f);

  float speed = KeyboardHandler_hasKey(menu->keyboardH, SDLK_LSHIFT) ? 5.f / 6.f : 5.f / 60.f;

  SDL_FPoint mouseM = MouseHandler_getMovement(menu->mouseH);

  camera_rotate(cam, mouseM.x * 0.01f, mouseM.y * 0.01f);
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_W)) camera_moveRel(cam, vertex(0, 0, speed));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_S)) camera_moveRel(cam, vertex(0, 0, -speed));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_A)) camera_moveRel(cam, vertex(-speed, 0, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_D)) camera_moveRel(cam, vertex(speed, 0, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_SPACE)) camera_moveRel(cam, vertex(0, -speed, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_LCTRL)) camera_moveRel(cam, vertex(0, speed, 0));
}

void render(SDL_Renderer *rend, void **args){
  SDL_Time *dt = args[0];
  Camera *cam = args[1];
  MenuState *menu = args[2];
  Object *objs = args[3];
  int *objCount = args[4];
  Object *stress = args[5];

  int w, h;
  SDL_GetRenderOutputSize(rend, &w, &h);
  float cx = w / 2.f, cy = h / 2.f;
  SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
  SDL_RenderClear(rend);

  for(int i = 0; i < objCount[0]; ++i)
  object_render(objs + i, rend, cam, cx, cy);
  
  //for(int i = 0; i < booboo; ++i)
  //object_render(stress + i, rend, cam, cx, cy);
}

int main(){
  SDL_Window *win;
  SDL_Renderer *rend;
  SDL_CreateWindowAndRenderer("3D graphics", 1360, 720, SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_MOUSE_RELATIVE_MODE | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL, &win, &rend);

  SDL_Time start, end = 0, dtime;
  SDL_GetCurrentTime(&start);
  SDL_srand(start);

  Vertex cryVert[6];

  Vertex cubeVert[8];

  Vertex icoVert[12];

  Polygon cryPoly[8];

  Polygon cubePoly[12];

  Polygon icoPoly[20];

  Model cryModel = crystal_gen(cryVert, cryPoly, 0.01f);
  Model cubeModel = cube_gen(cubeVert, cubePoly);
  Model icoModel = icosphere_gen(icoVert, icoPoly, 0);

  KeyboardHandler kbHandler = {0};
  MouseHandler moHandler = {0};
  MenuState menu = MenuState_new(NULL, NULL, &kbHandler, &moHandler, 0);
  Camera cam = camera(vertex(0, 0, 0), 200.f, 0.01f, 500.f);

  Color colors[] = {
    color(-1, 0, 0, -1),
    color(0, -1, 0, -1),
    color(0, 0, -1, -1),
    color(-1, -1, -1, -1)
  };
  /* Stress test *
  Object stress[booboo];
  for(int i = 0; i < booboo; ++i){
    int cmod = i % arrLen(colors), mod = i % boo, damp = i / boo;
    stress[i] = object(&cubeModel, colors + cmod, vertex(200.f * mod, 400.f * damp, 200.f), 100.f);
  }
  */
  Object objs[] = {
    object(&cryModel, colors+3, vertex(0, 0, 0), 10.f),
    object(&cubeModel, colors+3, vertex(110.f, 0, 15.f), 1.f),
    object(&icoModel, colors+3, vertex(-110.f, 0, 0), 10.f)
  };
  int objLen = arrLen(objs);
  
  void *args[] = {&dtime, &cam, &menu, objs, &objLen};
  
  SDL_Event eve = {0};
  while(eve.type != SDL_EVENT_QUIT){
    dtime = start - end;
    if(dtime >= 16000000){
      end = start;
      handleEvents(&eve, &menu);

      tick(rend, args);
      render(rend, args);

      SDL_RenderPresent(rend);
    }
    SDL_GetCurrentTime(&start);
    SDL_Delay(1);
  }
  
  SDL_Log("Normal execution");
}
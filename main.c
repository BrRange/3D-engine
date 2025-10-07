#include <SDL3/SDL.h>
#include "fold.c"
#include "arena.c"
#include "sdlFrame.c"
#include "icosphere.c"
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

  object_rotateY(objs, deltaT / SDL_PI_F / 200.f);

  float speed = KeyboardHandler_hasKey(menu->keyboardH, SDLK_LSHIFT) ? 1.f / 6.f : 1.f / 60.f;

  SDL_FPoint mouseM = MouseHandler_getMovement(menu->mouseH);

  camera_rotate(cam, mouseM.x * 0.01f, mouseM.y * 0.01f);
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_W)) camera_move(cam, vertex(0, 0, speed));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_S)) camera_move(cam, vertex(0, 0, -speed));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_A)) camera_move(cam, vertex(-speed, 0, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_D)) camera_move(cam, vertex(speed, 0, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_SPACE)) camera_move(cam, vertex(0, -speed, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_LCTRL)) camera_move(cam, vertex(0, speed, 0));

  object_move(objs+1, vertex(cam->pos.x - objs[1].pos.x, cam->pos.y - objs[1].pos.y, cam->pos.z + 5.f - objs[1].pos.z));
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

  Vertex cubeVert[] = {
    vertex(0.5f, 0.5f, 0.5f),
    vertex(0.5f, 0.5f, -0.5f),
    vertex(0.5f, -0.5f, 0.5f),
    vertex(0.5f, -0.5f, -0.5f),
    vertex(-0.5f, 0.5f, 0.5f),
    vertex(-0.5f, 0.5f, -0.5f),
    vertex(-0.5f, -0.5f, 0.5f),
    vertex(-0.5f, -0.5f, -0.5f),
  };

  Vertex cryVert[] = {
    vertex(0.5f, 0.f, 0.5f),
    vertex(0.5f, 0.f, -0.5f),
    vertex(-0.5f, 0.f, 0.5f),
    vertex(-0.5f, 0.f, -0.5f),
    vertex(0.f, 1.f, 0.f),
    vertex(0.f, -1.f, 0.f),
  };

  Vertex icoVert[12];

  Poly cryPoly[] = {
    poly(0, 1, 4, 0),
    poly(1, 3, 4, 0),
    poly(2, 3, 4, 0),
    poly(0, 2, 4, 0),
    poly(0, 1, 5, 0),
    poly(1, 3, 5, 0),
    poly(2, 3, 5, 0),
    poly(0, 2, 5, 0),
  };

  Poly cubePoly[] = {
    poly(0, 1, 2, 0),
    poly(1, 2, 3, 0),
    poly(0, 1, 4, 0),
    poly(1, 4, 5, 0),
    poly(0, 2, 4, 0),
    poly(2, 4, 6, 0),
    poly(3, 6, 7, 0),
    poly(2, 3, 6, 0),
    poly(4, 5, 6, 0),
    poly(5, 6, 7, 0),
    poly(3, 5, 7, 0),
    poly(1, 3, 5, 0),
  };

  Poly icoPoly[20];

  Model cryModel = model(cryVert, arrLen(cryVert), cryPoly, arrLen(cryPoly));
  Model cubeModel = model(cubeVert, arrLen(cubeVert), cubePoly, arrLen(cubePoly));
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
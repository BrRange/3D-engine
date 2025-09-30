#include <SDL3/SDL.h>
#include "fold.c"
#include "arena.c"
#include "sdlFrame.c"
#include "eventHandler.h"



void tick(SDL_Renderer *rend, void **args){
  SDL_Time *dt = args[0];
  Object *cube = args[1];
  Camera *cam = args[2];
  MenuState *menu = args[3];

  float speed = KeyboardHandler_hasKey(menu->keyboardH, SDLK_LSHIFT) ? 50.f : 10.f;

  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_LEFT)) camera_rotate(cam, -0.01f, 0);
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_RIGHT)) camera_rotate(cam, 0.01f, 0);
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_UP)) camera_rotate(cam, 0, -0.01f);
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_DOWN)) camera_rotate(cam, 0, 0.01f);
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_W)) camera_move(cam, vertex(0, 0, speed));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_S)) camera_move(cam, vertex(0, 0, -speed));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_A)) camera_move(cam, vertex(-speed, 0, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_D)) camera_move(cam, vertex(speed, 0, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_SPACE)) camera_move(cam, vertex(0, -speed, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_LCTRL)) camera_move(cam, vertex(0, speed, 0));
}

void render(SDL_Renderer *rend, void **args){
  SDL_Time *dt = args[0];
  Object *cube = args[1];
  Camera *cam = args[2];
  MenuState *menu = args[3];
  Object *cube2 = args[4];
  Object *cube3 = args[5];
  Object *cube4 = args[6];

  int w, h;
  SDL_GetRenderOutputSize(rend, &w, &h);
  float cx = w / 2.f, cy = h / 2.f;
  SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
  SDL_RenderClear(rend);

  object_render(cube, rend, cam, cx, cy);
  object_render(cube2, rend, cam, cx, cy);
  object_render(cube3, rend, cam, cx, cy);
  object_render(cube4, rend, cam, cx, cy);
}

int main(){
  SDL_Window *win;
  SDL_Renderer *rend;
  SDL_CreateWindowAndRenderer("3D graphics", 1360, 720, SDL_WINDOW_MOUSE_GRABBED | SDL_WINDOW_OPENGL, &win, &rend);

  SDL_Time start, end = 0, dtime;
  SDL_GetCurrentTime(&start);
  SDL_srand(start);

  Vertex vertices[] = {
    vertex(0.5f, 0.5f, 0.5f),
    vertex(-0.5f, 0.5f, 0.5f),
    vertex(0.5f, -0.5f, 0.5f),
    vertex(-0.5f, -0.5f, 0.5f),
    vertex(0.5f, 0.5f, -0.5f),
    vertex(-0.5f, 0.5f, -0.5f),
    vertex(0.5f, -0.5f, -0.5f),
    vertex(-0.5f, -0.5f, -0.5f),
  };

  Color colors[] = {
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1),
    color(-1, -1, -1, -1)
  };

  Poly polys[] = {
    poly(vertices + 0, vertices + 1, vertices + 2),
    poly(vertices + 1, vertices + 2, vertices + 3),
    poly(vertices + 0, vertices + 1, vertices + 4),
    poly(vertices + 1, vertices + 4, vertices + 5),
    poly(vertices + 0, vertices + 2, vertices + 4),
    poly(vertices + 2, vertices + 4, vertices + 6),
    poly(vertices + 1, vertices + 3, vertices + 5),
    poly(vertices + 3, vertices + 5, vertices + 7),
    poly(vertices + 2, vertices + 3, vertices + 6),
    poly(vertices + 3, vertices + 6, vertices + 7),
    poly(vertices + 4, vertices + 5, vertices + 6),
    poly(vertices + 5, vertices + 6, vertices + 7)
  };
  Camera cam = camera(vertex(0, 0, 0), 0.1f, 200.f, 90.f);
  KeyboardHandler kbHandler = {0};
  MouseHandler moHandler = {0};
  MenuState menu = MenuState_new(NULL, NULL, &kbHandler, &moHandler, 0);
  Model mdl = model(
    vertices,
    sizeof(vertices) / sizeof(*vertices),
    polys,
    colors,
    sizeof(polys) / sizeof(*polys)
  );
  Object cube =  object(&mdl, vertex(0.f, 0, 200), 100.f);
  Object cube2 = object(&mdl, vertex(100, 0, 200), 100.f);
  Object cube3 = object(&mdl, vertex(200, 0, 200), 100.f);
  Object cube4 = object(&mdl, vertex(300, 0, 200), 100.f);
  void *args[] = {&dtime, &cube, &cam, &menu, &cube2, &cube3, &cube4};
  
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
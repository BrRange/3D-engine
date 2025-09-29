#include "SDL3/SDL.h"
#include "fold.c"
#include "arena.c"
#include "sdlFrame.c"
#include "sdlFrameGUI.c"
#include "eventHandler.h"



void tick(SDL_Renderer *rend, void **args){
  SDL_Time *dt = args[0];
  Poly *polys = args[1];
  Color *colors = args[2];
  Vertex *vertices = args[3];
  Camera *cam = args[4];
  MenuState *menu = args[5];

  SDL_FPoint mouseMove = MouseHandler_getMovement(menu->mouseH);

  SDL_Log("%.2f %.2f", cam->yaw, cam->pitch);

  camera_rotate(cam, mouseMove.x / -10.f, mouseMove.y / -10.f);

  float speed = KeyboardHandler_hasKey(menu->keyboardH, SDLK_LSHIFT) ? 10.f : 1.f;

  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_W)) camera_move(cam, vertex(0, 0, speed));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_S)) camera_move(cam, vertex(0, 0, -speed));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_A)) camera_move(cam, vertex(-speed, 0, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_D)) camera_move(cam, vertex(speed, 0, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_SPACE)) camera_move(cam, vertex(0, -speed, 0));
  if(KeyboardHandler_hasKey(menu->keyboardH, SDLK_LCTRL)) camera_move(cam, vertex(0, speed, 0));

  float dtf = *dt * 1e-9f;
  for(int i = 0; i < 8; ++i){
    vertex_rotX(vertices + i, dtf);
    vertex_rotY(vertices + i, 2.f * dtf);
  }
}

void render(SDL_Renderer *rend, void **args){
  SDL_Time *dt = args[0];
  Poly *polys = args[1];
  Color *colors = args[2];
  Vertex *vertices = args[3];
  Camera *cam = args[4];
  MenuState *menu = args[5];
  
  SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
  SDL_RenderClear(rend);

  for(int i = 0; i < 12; ++i)
    poly_render(rend, cam, polys + i, colors[i / 2]);
}

int main(){
  SDL_Window *win;
  SDL_Renderer *rend;
  SDL_CreateWindowAndRenderer("3D graphics", 1360, 720, SDL_WINDOW_OPENGL, &win, &rend);

  SDL_Time start, end = 0, dtime;
  SDL_GetCurrentTime(&start);
  SDL_srand(start);

  Vertex vertices[] = {
    vertex(1, 1, 1),
    vertex(-1, 1, 1),
    vertex(1, -1, 1),
    vertex(-1, -1, 1),
    vertex(1, 1, -1),
    vertex(-1, 1, -1),
    vertex(1, -1, -1),
    vertex(-1, -1, -1),
  };
  Color colors[] = {
    color(-1, 0, 0, -1),
    color(0, -1, 0, -1),
    color(-1, -1, 0, -1),
    color(0, 0, -1, -1),
    color(-1, 0, -1, -1),
    color(0, -1, -1, -1)
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
  Camera cam = camera(vertex(-680.f, -360.f, 0.f), 50.f);
  KeyboardHandler kbHandler = {0};
  MouseHandler moHandler = {0};
  MenuState menu = MenuState_new(NULL, NULL, &kbHandler, &moHandler, 0);
  void *args[] = {&dtime, polys, colors, vertices, &cam, &menu};
  
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
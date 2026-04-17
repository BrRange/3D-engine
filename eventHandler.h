#include "include/dataType/gamectrl.h"

bool handleEvents(KeyboardHandler *keyboardH, MouseHandler *mouseH){
  SDL_Event eve;
  while(SDL_PollEvent(&eve))
  switch(eve.type){
  case SDL_EVENT_QUIT:
  return false;

  case SDL_EVENT_WINDOW_DESTROYED:
    SDL_Quit();
  break;

  case SDL_EVENT_MOUSE_MOTION:
    mouseH_setPos(mouseH, eve.motion.x, eve.motion.y);
    mouseH_move(mouseH, eve.motion.xrel, eve.motion.yrel);
  break;

  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    mouseH_press(mouseH, eve.button.button);
  break;

  case SDL_EVENT_MOUSE_BUTTON_UP:
    mouseH_release(mouseH, eve.button.button);
  break;

  case SDL_EVENT_MOUSE_WHEEL:
    mouseH_scroll(mouseH, eve.wheel.y);
  break;

  case SDL_EVENT_KEY_DOWN:
    keyboardH_press(keyboardH, eve.key.key);
  break;

  case SDL_EVENT_KEY_UP:
    keyboardH_release(keyboardH, eve.key.key);
  break;

  case SDL_EVENT_WINDOW_FOCUS_LOST:
    keyboardH_clear(keyboardH);
    mouseH_clear(mouseH);
  break;
  }
  return true;
}
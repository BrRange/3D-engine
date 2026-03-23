#include "gamectrl.h"

void keyboardH_press(KeyboardHandler *kbH, u32 key) {
  if (kbH->used >= sizeof(kbH->keys) / sizeof(*kbH->keys)) return;
  for (u32 i = 0; i < kbH->used; ++i) {
    if (kbH->keys[i] == key) return;
  }
  kbH->keys[kbH->used++] = key;
}


void keyboardH_release(KeyboardHandler *kbH, u32 key) {
  for (u32 i = 0; i < kbH->used; ++i) {
    if (kbH->keys[i] == key) {
      for (u32 j = i; j < kbH->used - 1; ++j)
        kbH->keys[j] = kbH->keys[j + 1];
      kbH->used--;
      kbH->keys[kbH->used] = 0;
      return;
    }
  }
}

void keyboardH_clear(KeyboardHandler *kbH){
  *kbH = (KeyboardHandler){0};
}

bool keyboardH_has(KeyboardHandler *kbH, u32 key){
  for(u32 i = 0; i < sizeof(kbH->keys) / sizeof(kbH->keys[0]); ++i)
  if(kbH->keys[i] == key) return true;
  return false;
}

void mouseH_setPos(MouseHandler *mouseH, float x, float y){
  mouseH->pos = (Vec2){.x = x, .y = y};
}

void mouseH_move(MouseHandler *mouseH, float dx, float dy){
  mouseH->move = (Vec2){.x = dx, .y = dy};
}

Vec2 mouseH_getPos(MouseHandler *mouseH){
  return mouseH->pos;
}

Vec2 mouseH_getMovement(MouseHandler *mouseH){
  Vec2 ret = mouseH->move;
  mouseH->move = (Vec2){0};
  return ret;
}

void mouseH_press(MouseHandler *mouseH, u8 button){
  SDL_assert(button > 0 && button <= 8);
  button = 1 << (button - 1);
  mouseH->down |= button;
}

void mouseH_release(MouseHandler *mouseH, u8 button){
  SDL_assert(button > 0 && button <= 8);
  button = 1 << (button - 1);
  mouseH->up |= button;
}

u8 mouseH_has(MouseHandler *mouseH, u8 button){
  if(mouseH->down & button){
    if(mouseH->up & button){
      mouseH->down &= ~button;
      mouseH->up &= ~button;
      return MOUSE_CLICK;
    }
    return MOUSE_HOLD;
  }
  return MOUSE_NONE;
}

void mouseH_scroll(MouseHandler *mouseH, float scroll){
  mouseH->scroll = scroll;
}

float mouseH_getScroll(MouseHandler *mouseH){
  float scroll = mouseH->scroll;
  mouseH->scroll = 0.f;
  return scroll;
}

void mouseH_clear(MouseHandler *mouseH){
  *mouseH = (MouseHandler){.pos = mouseH->pos};
}
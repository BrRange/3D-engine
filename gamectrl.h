#ifndef GAME_CONTROL_H_
#define GAME_CONTROL_H_

#ifndef RUSTY_DEFINITION_
#define RUSTY_DEFINITION_

#define arrLen(_arr) (sizeof(_arr) / sizeof*(_arr))
#define deref(_ptr, _type) (*(_type*)(_ptr))
#define pointer(_type...) typeof(typeof(_type)*)
#define defer(_freeFn) __attribute__((cleanup(_freeFn)))

#include <stdint.h>
#include <stdbool.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef intptr_t isz;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t usz;
typedef float f32;
typedef double f64;

#endif

#ifndef VECTOR_H_

typedef struct Vec2{
  float x, y;
} Vec2;

#endif

struct KeyboardHandler{
  u32 keys[6];
  u32 used;
};
typedef struct KeyboardHandler KeyboardHandler;

/** \attention Should be called by the event handler */
void keyboardH_press(KeyboardHandler*, u32);

/** \attention Should be called by the event handler */
void keyboardH_release(KeyboardHandler*, u32);

bool keyboardH_has(KeyboardHandler*, u32);

void keyboardH_clear(KeyboardHandler*);

#define MOUSEB_LEFT (1 << 0)
#define MOUSEB_MID (1 << 1)
#define MOUSEB_RIGHT (1 << 2)
#define MOUSEB_EXTRA(_x) (1 << (3 + (_x)))
#define MOUSE_NONE 0
#define MOUSE_CLICK 1
#define MOUSE_HOLD 2

struct MouseHandler{
  Vec2 pos, move;
  float scroll;
  u8 up, down;
};
typedef struct MouseHandler MouseHandler;

/** \attention Should be called by the event handler */
void mouseH_setPos(MouseHandler*, float x, float y);

/** \attention Should be called by the event handler */
void mouseH_move(MouseHandler*, float dx, float dy);

Vec2 mouseH_getPos(MouseHandler*);

/** \attention Should be called once per frame */
Vec2 mouseH_getMovement(MouseHandler*);

/** \attention Should be called by the event handler */
void mouseH_press(MouseHandler*, u8 button);

/** \attention Should be called by the event handler */
void mouseH_release(MouseHandler*, u8 button);

/** \attention Should be called once per frame */
u8 mouseH_has(MouseHandler*, u8 button);

void mouseH_scroll(MouseHandler*, float scroll);

/** \attention Should be called once per frame */
float mouseH_getScroll(MouseHandler*);

void mouseH_clear(MouseHandler*);

#endif
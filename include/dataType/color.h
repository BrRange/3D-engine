#ifndef DATATYPE_COLOR_H_
#define DATATYPE_COLOR_H_

#include <SDL3/SDL_render.h>
#include "include/dataType/vector.h"

typedef union Color{
  f32 color[3];
  struct{
    f32 r, g, b;
  };
} Color;

Color color_new(f32 r, f32 g, f32 b);

typedef union PixelColor{
  u8 color[4];
  struct{
    u8 r, g, b, a;
  };
} PixelColor;

PixelColor pixelColor_new(u8 r, u8 g, u8 b, u8 a);

typedef struct Canvas{
  PixelColor *pixel;
  f32 *zBuffer;
  SDL_Texture *tex;
  i32 w, h;
} Canvas;

void shader_pixel(Canvas *canv, Vec3 vertex[3], Color color);

Canvas canvas_new(SDL_Renderer *rend, u32 w, u32 h);

void canvas_render(Canvas *canv, SDL_Renderer *rend);

void canvas_destroy(Canvas *canv);

#endif
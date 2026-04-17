#include "include/dataType/color.h"

Color color_new(f32 r, f32 g, f32 b){
  Color c = {
    .r = r,
    .g = g,
    .b = b
  };
  return c;
}

PixelColor pixelColor_new(u8 r, u8 g, u8 b, u8 a){
  PixelColor p = {
    .r = r,
    .g = g,
    .b = b,
    .a = a
  };
  return p;
}

void shader_pixel(Canvas *canv, Vec3 *vertex, Color color){
  i32 bounds[4];
  f32 cx = canv->w / 2, cy = canv->h / 2;

  Vec2
  a = {vertex[0].x + cx, vertex[0].y + cy},
  b = {vertex[1].x + cx, vertex[1].y + cy},
  c = {vertex[2].x + cx, vertex[2].y + cy};
  
  if(vec2_edge(a, b, c) <= 0.f) return;
  
  vec2_bound(a, b, c, bounds);
  
  bounds[0] = SDL_max(bounds[0], 0);
  bounds[1] = SDL_min(bounds[1], canv->w - 1);
  bounds[2] = SDL_max(bounds[2], 0);
  bounds[3] = SDL_min(bounds[3], canv->h - 1);

  const Vec3
  xdiff = vec3_new(b.y - a.y, c.y - b.y, a.y - c.y),
  ydiff = vec3_new(b.x - a.x, c.x - b.x, a.x - c.x);

  f32 iAB = (b.x * a.y - a.x * b.y) + xdiff.x * bounds[0] - ydiff.x * bounds[2];
  f32 iBC = (c.x * b.y - b.x * c.y) + xdiff.y * bounds[0] - ydiff.y * bounds[2];
  f32 iCA = (a.x * c.y - c.x * a.y) + xdiff.z * bounds[0] - ydiff.z * bounds[2];

  
  for(i32 row = bounds[2]; row < bounds[3]; ++row){
    f32 AB = iAB;
    f32 BC = iBC;
    f32 CA = iCA;
    for(i32 col = bounds[0]; col < bounds[1]; ++col){
      usz idx = (usz)row * canv->w + col;

      if(AB < 0.f) goto skip;
      if(BC < 0.f) goto skip;
      if(CA < 0.f) goto skip;

      f32 bary = 1.f / (AB + BC + CA);
      
      Vec3
      weight = vec3_new(BC * bary, CA * bary, AB * bary),
      depth = vec3_new(1.f / vertex[0].z, 1.f / vertex[1].z, 1.f / vertex[2].z);
      
      f32 thisZ = 1.f / vec3_dot(weight, depth);
      
      if(thisZ >= canv->zBuffer[idx]) goto skip;
      PixelColor pcolor = pixelColor_new(255 * color.r, 255 * color.g, 255 * color.b, 255);
      canv->pixel[idx] = pcolor;
      canv->zBuffer[idx] = thisZ;

      skip:
      AB += xdiff.x;
      BC += xdiff.y;
      CA += xdiff.z;
    }
    iAB -= ydiff.x;
    iBC -= ydiff.y;
    iCA -= ydiff.z;
  }
}

Canvas canvas_new(SDL_Renderer *rend, u32 w, u32 h){
  Canvas canv = {
    .pixel = NULL,
    .zBuffer = SDL_malloc(sizeof *canv.zBuffer * w * h),
    .tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h),
    .w = w,
    .h = h
  };
  return canv;
}

void canvas_clear(Canvas *canv){
  usz len;
  SDL_LockTexture(canv->tex, NULL, (void**)&canv->pixel, (int*)&len);
  len = (usz)canv->w * canv->h;
  SDL_memset4(canv->zBuffer, 0x7f800000, len);
  SDL_memset4(canv->pixel, 0, len);
}

void canvas_render(Canvas *canv, SDL_Renderer *rend){
  SDL_UnlockTexture(canv->tex);
  SDL_RenderTexture(rend, canv->tex, NULL, NULL);
}

void canvas_destroy(Canvas *canv){
  SDL_free(canv->zBuffer);
  SDL_DestroyTexture(canv->tex);
}
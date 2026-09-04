#include "dataType/canvas.h"
#include "entity/object.h"

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

void shader_pixel(Canvas *canv){
  i32 bounds[4];
  f32 cx = canv->w / 2, cy = canv->h / 2;

  for(u32 i = 0; i < canv->vertices.len; ++i){

    Vertex *vertex = darray_get(canv->vertices, i);
    SDL_Surface *UVmap = *(SDL_Surface**)darray_get(canv->uvSurfaces, i);
    
    Vec2
    a = {vertex[0].coord[0] + cx, vertex[0].coord[1] + cy},
    b = {vertex[1].coord[0] + cx, vertex[1].coord[1] + cy},
    c = {vertex[2].coord[0] + cx, vertex[2].coord[1] + cy};
  
    if(vec2_edge(a, b, c) <= 0.f) continue;
    
    vec2_bound(a, b, c, bounds);
    
    bounds[0] = SDL_max(bounds[0], 0);
    bounds[1] = SDL_min(bounds[1], canv->w - 1);
    bounds[2] = SDL_max(bounds[2], 0);
    bounds[3] = SDL_min(bounds[3], canv->h - 1);
    
    const Vec4
    xdiff = vec4_new(b.y - a.y, c.y - b.y, a.y - c.y),
    ydiff = vec4_new(b.x - a.x, c.x - b.x, a.x - c.x),
    depth = vec4_new(vertex[0].coord[3], vertex[1].coord[3], vertex[2].coord[3]);
    
    f32 iAB = (b.x * a.y - a.x * b.y) + xdiff[0] * bounds[0] - ydiff[0] * bounds[2];
    f32 iBC = (c.x * b.y - b.x * c.y) + xdiff[1] * bounds[0] - ydiff[1] * bounds[2];
    f32 iCA = (a.x * c.y - c.x * a.y) + xdiff[2] * bounds[0] - ydiff[2] * bounds[2];
    
    f32 bary = 1.f / (iAB + iBC + iCA);

    SDL_Rect UVsize;
    SDL_GetSurfaceClipRect(UVmap, &UVsize);
    
    for(i32 row = bounds[2]; row < bounds[3]; ++row){
      f32 AB = iAB;
      f32 BC = iBC;
      f32 CA = iCA;
      for(i32 col = bounds[0]; col < bounds[1]; ++col){
        usz idx = (usz)row * canv->w + col;
        
        if(AB < 0.f) goto skip;
        if(BC < 0.f) goto skip;
        if(CA < 0.f) goto skip;
        
        Vec4 weight = vec4_new(BC * bary, CA * bary, AB * bary);
        
        f32 thisInv = vec4_dot(weight, depth);
        f32 thisZ = 1.f / thisInv;
        
        if(thisZ >= canv->zBuffer[idx]) goto skip;
        
        Vec4 W = vec4_new(
          vertex[0].coord[3],
          vertex[1].coord[3],
          vertex[2].coord[3]
        );

        W *= weight;
        
        Vec4 pixel_normal = vertex[0].normal * W[0] + vertex[1].normal * W[1] + vertex[2].normal * W[2];
        pixel_normal *= thisZ;
        
        const Vec4 light = vec4_normal(vec4_new(1, -1, 1));
        
        f32 incidence = 255 * vec4_dot(light, pixel_normal);
        incidence = SDL_clamp(incidence, 32, 255);
        
        Vec2 pixel_uv = vec2_mul(vertex[0].uv, W[0]);
        pixel_uv = vec2_add(pixel_uv, vec2_mul(vertex[1].uv, W[1]));
        pixel_uv = vec2_add(pixel_uv, vec2_mul(vertex[2].uv, W[2]));
        pixel_uv = vec2_mul(pixel_uv, thisZ);
        Color color;
        SDL_ReadSurfacePixelFloat(UVmap, pixel_uv.x * UVsize.w, pixel_uv.y * UVsize.h, &color.r, &color.g, &color.b, NULL);
        PixelColor pcolor = pixelColor_new(incidence * color.r, incidence * color.g, incidence *color.b, 255);
        canv->pixel[idx] = pcolor;
        canv->zBuffer[idx] = thisZ;

        skip:
        AB += xdiff[0];
        BC += xdiff[1];
        CA += xdiff[2];
      }
      iAB -= ydiff[0];
      iBC -= ydiff[1];
      iCA -= ydiff[2];
    }
  }
}

Canvas canvas_new(SDL_Renderer *rend, u32 w, u32 h){
  Canvas canv = {
    .pixel = NULL,
    .zBuffer = SDL_malloc(sizeof *canv.zBuffer * w * h),
    .tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h),
    .w = w,
    .h = h,
    .vertices = darray_new(sizeof(Vertex[3])),
    .uvSurfaces = darray_new(sizeof(SDL_Surface*))
  };
  SDL_SetTextureScaleMode(canv.tex, SDL_SCALEMODE_NEAREST);
  return canv;
}

void canvas_clear(Canvas *canv){
  usz len;
  SDL_LockTexture(canv->tex, NULL, (void**)&canv->pixel, (int*)&len);
  len = (usz)canv->w * canv->h;
  SDL_memset4(canv->zBuffer, 0x7f800000, len);
  SDL_memset4(canv->pixel, 0, len);
  canv->vertices.len = 0;
  canv->uvSurfaces.len = 0;
}

void canvas_render(Canvas *canv, SDL_Renderer *rend){
  shader_pixel(canv);
  SDL_UnlockTexture(canv->tex);
  SDL_RenderTexture(rend, canv->tex, NULL, NULL);
}

void canvas_destroy(Canvas *canv){
  darray_destroy(&canv->vertices);
  darray_destroy(&canv->uvSurfaces);
  SDL_free(canv->zBuffer);
  SDL_DestroyTexture(canv->tex);
}
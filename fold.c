#include "fold.h"
#include <math.h>

Camera camera(const Vec3 v, f32 farPlane, f32 nearPlane, f32 fieldView){
  Camera cam = {
    .pos = v,
    .farPlane = farPlane,
    .nearPlane = nearPlane,
    .fieldView = fieldView
  };
  return cam;
}

Vec3 camera_viewVec3(const Camera *cam){
  f32 ps, pc, ys, yc;
  sincosf(cam->yaw, &ys, &yc);
  sincosf(cam->pitch, &ps, &pc);
  
  return vec3_new(pc * ys, ps, pc * yc);
}

void camera_rotate(Camera *cam, f32 dYaw, f32 dPitch){
  cam->yaw = SDL_fmodf(cam->yaw + dYaw, 2.f * SDL_PI_F);
  cam->pitch = SDL_min(SDL_max(cam->pitch + dPitch, SDL_PI_F / -2.01f), SDL_PI_F / 2.01f);
  cam->rot = quat_new(cam->yaw, vec3_new(0, -1, 0));
  cam->rot = quat_compose(quat_new(cam->pitch, vec3_new(1, 0, 0)), cam->rot);
}

void camera_rotateUnbound(Camera *cam, f32 dYaw, f32 dPitch){
  cam->yaw = SDL_fmodf(cam->yaw + dYaw, 2.f * SDL_PI_F);
  cam->pitch = SDL_fmodf(cam->pitch + dPitch, 2.f * SDL_PI_F);
  cam->rot = quat_new(cam->yaw, vec3_new(0, -1, 0));
  cam->rot = quat_compose(quat_new(cam->pitch, vec3_new(1, 0, 0)), cam->rot);
}

void camera_moveAbs(Camera *cam, const Vec3 dPos){
  cam->pos = vec3_add(cam->pos, dPos);
}

void camera_moveRel(Camera *cam, const Vec3 dPos){
  f32 ps, pc, ys, yc;
  sincosf(cam->pitch, &ps, &pc);
  sincosf(cam->yaw, &ys, &yc);
  cam->pos.x += dPos.x * yc
                + dPos.y * -ys * ps
                + dPos.z * pc * ys;

    cam->pos.y += dPos.y * pc + dPos.z * ps;
    cam->pos.z += dPos.x * -ys + dPos.y * -yc * ps + dPos.z * pc * yc;
}

Vec3 vec3_onCamera(const Vec3 v, const Camera *restrict cam, const Vec3 offset, const Quaternion rot, f32 scale){
  Vec3 dv = vec3_rotate(v, rot);
  
  dv.x = dv.x * scale - cam->pos.x + offset.x,
  dv.y = dv.y * scale - cam->pos.y + offset.y,
  dv.z = dv.z * scale - cam->pos.z + offset.z;

  return vec3_rotate(dv, cam->rot);
}

Color color(u8 r, u8 g, u8 b, u8 a){
  Color c = {
    .r = r,
    .g = g,
    .b = b,
    .a = a
  };
  return c;
}

Polygon polygon_new(u16 idx0, u16 idx1, u16 idx2, u16 colorIndex){
  Polygon p = {
    .idx = {idx0, idx1, idx2},
    .colorIndex = colorIndex
  };
  return p;
}

Model model(Vec3 *vec3_new, size_t vec3Count, Polygon *polygon_new, size_t polyCount){
  Model mdl = {
    .vec3 = vec3_new,
    .vec3Count = vec3Count,
    .polygon = polygon_new,
    .polyCount = polyCount
  };
  return mdl;
}

Object object_new(Model *model, Color *palette, const Vec3 pos, f32 scale){
  Object obj = {
    .model = model,
    .palette = palette,
    .pos = pos,
    .scale = scale,
    .rot = {.r = 1}
  };
  return obj;
}

void object_rotate(Object *obj, Quaternion quat){
  obj->rot = quat_compose(obj->rot, quat);
}

void object_move(Object *obj, const Vec3 dv){
  obj->pos.x += dv.x;
  obj->pos.y += dv.y;
  obj->pos.z += dv.z;
}

void object_render(Object *obj, Canvas *canv, Camera *cam){
  Vec3 *vert = obj->model->vec3, clipped[3], unclipped[3];
  u8 clipCount, unclipCount;

  for(size_t i = 0; i < obj->model->polyCount; ++i){
    clipCount = unclipCount = 0;
    Polygon polygon_new = obj->model->polygon[i];

    Vec3 proj[4] = {
      vec3_onCamera(polygon_new.idx[0][vert], cam, obj->pos, obj->rot, obj->scale),
      vec3_onCamera(polygon_new.idx[1][vert], cam, obj->pos, obj->rot, obj->scale),
      vec3_onCamera(polygon_new.idx[2][vert], cam, obj->pos, obj->rot, obj->scale)
    };

    f32 canvDiag = hypotf(canv->w, canv->h);

    proj[0].x *= cam->fieldView * canvDiag, proj[0].y *= cam->fieldView * canvDiag;
    proj[1].x *= cam->fieldView * canvDiag, proj[1].y *= cam->fieldView * canvDiag;
    proj[2].x *= cam->fieldView * canvDiag, proj[2].y *= cam->fieldView * canvDiag;
    
    Color color = obj->palette[polygon_new.colorIndex];

    for(u8 j = 0; j < 3; ++j)
    if(proj[j].z <= cam->nearPlane) clipped[clipCount++] = proj[j];
    else unclipped[unclipCount++] = proj[j];

    bool extraVec3 = false;
    switch(clipCount){
    case 0:
      for(u8 j = 0; j < 3; ++j)
        proj[j].x /= proj[j].z, proj[j].y /= proj[j].z;
    break;
    case 1:
      extraVec3 = true;
      proj[0] = unclipped[0];
      proj[1] = unclipped[1];
      proj[2] = vec3_getClip(clipped[0], unclipped[0], cam->nearPlane);
      proj[3] = vec3_getClip(clipped[0], unclipped[1], cam->nearPlane);
      for(u8 j = 0; j < 2; ++j)
        proj[j].x /= proj[j].z, proj[j].y /= proj[j].z;
    break;
    case 2:{
      proj[0] = unclipped[0];
      proj[1] = vec3_getClip(clipped[0], unclipped[0], cam->nearPlane);
      proj[2] = vec3_getClip(clipped[1], unclipped[0], cam->nearPlane);
      proj[0].x /= proj[0].z, proj[0].y /= proj[0].z;
    }
    break;
    default:
      continue;
    }

    u32 bounds[4];
    f32 cx = canv->w / 2.f, cy = canv->h / 2.f;

    Vec2
    a = {proj[0].x + cx, proj[0].y + cy},
    b = {proj[1].x + cx, proj[1].y + cy},
    c = {proj[2].x + cx, proj[2].y + cy};

    if(vec2_edge(c, b, a) <= 0.f) continue;

    vec2_bound(a, b, c, bounds);

    bounds[0] = SDL_max(bounds[0], 0);
    bounds[1] = SDL_min(bounds[1], canv->w - 1);
    bounds[2] = SDL_max(bounds[2], 0);
    bounds[3] = SDL_min(bounds[3], canv->h - 1);

    for(i32 row = bounds[2]; row < bounds[3]; ++row)
    for(i32 col = bounds[0]; col < bounds[1]; ++col){
      usz idx = (usz)row * canv->w + col;
      
      Vec2 p = vec2_new(col, row);
      f32
      AB = vec2_edge(p, b, a),
      BC = vec2_edge(p, c, b),
      CA = vec2_edge(p, a, c);

      f32 bary = 1.f / (AB + BC + CA);
      
      Vec3 weight = vec3_new(BC * bary, CA * bary, AB * bary), depth = vec3_new(proj[0].z, proj[1].z, proj[2].z);

      f32 thisZ = vec3_dot(weight, depth);

      if(thisZ >= canv->zBuffer[idx]) continue;
      if(AB < 0.f || BC < 0.f || CA < 0.f) continue;
      canv->pixel[idx] = color;
      canv->zBuffer[idx] = thisZ;
    }
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
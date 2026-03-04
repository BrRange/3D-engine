#include "fold.h"
#include <SDL3_gfx/SDL3_gfxPrimitives.h>
#include <math.h>

Camera camera(Vertex v, f32 farPlane, f32 nearPlane, f32 fieldView){
  Camera cam = {
    .pos = v,
    .farPlane = farPlane,
    .nearPlane = nearPlane,
    .fieldView = fieldView
  };
  return cam;
}

Vertex camera_viewVertex(const Camera *cam){
  f32 ps, pc, ys, yc;
  sincosf(cam->yaw, &ys, &yc);
  sincosf(cam->pitch, &ps, &pc);
  
  return vertex_new(pc * ys, ps, pc * yc);
}

void camera_rotate(Camera *cam, f32 dYaw, f32 dPitch){
  cam->yaw = SDL_fmodf(cam->yaw + dYaw, 2.f * SDL_PI_F);
  cam->pitch = SDL_min(SDL_max(cam->pitch + dPitch, SDL_PI_F / -2.01f), SDL_PI_F / 2.01f);
  cam->rot = quat_new(cam->yaw, vertex_new(0, -1, 0));
  cam->rot = quat_compose(quat_new(cam->pitch, vertex_new(1, 0, 0)), cam->rot);
}

void camera_rotateUnbound(Camera *cam, f32 dYaw, f32 dPitch){
  cam->yaw = SDL_fmodf(cam->yaw + dYaw, 2.f * SDL_PI_F);
  cam->pitch = SDL_fmodf(cam->pitch + dPitch, 2.f * SDL_PI_F);
  cam->rot = quat_new(cam->yaw, vertex_new(0, -1, 0));
  cam->rot = quat_compose(quat_new(cam->pitch, vertex_new(1, 0, 0)), cam->rot);
}

void camera_moveAbs(Camera *cam, Vertex dPos){
  cam->pos = vertex_add(cam->pos, dPos);
}

void camera_moveRel(Camera *cam, Vertex dPos){
  f32 ps, pc, ys, yc;
  sincosf(cam->pitch, &ps, &pc);
  sincosf(cam->yaw, &ys, &yc);
  cam->pos.x += dPos.x * yc
                + dPos.y * -ys * ps
                + dPos.z * pc * ys;

    cam->pos.y += dPos.y * pc + dPos.z * ps;
    cam->pos.z += dPos.x * -ys + dPos.y * -yc * ps + dPos.z * pc * yc;
}

Vertex vertex_onCamera(const Vertex v, const Camera *restrict cam, const Vertex offset, const Quaternion rot, f32 scale){

  Vertex dv = vertex_rotate(v, rot);
  
  dv.x = dv.x * scale - cam->pos.x + offset.x,
  dv.y = dv.y * scale - cam->pos.y + offset.y,
  dv.z = dv.z * scale - cam->pos.z + offset.z;

  return vertex_rotate(dv, cam->rot);
}

Color color(f32 r, f32 g, f32 b, f32 a){
  Color c = {
    .rgba = {r, g, b, a}
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

Model model(Vertex *vertex_new, size_t vertexCount, Polygon *polygon_new, size_t polyCount){
  Model mdl = {
    .vertex = vertex_new,
    .vertexCount = vertexCount,
    .polygon = polygon_new,
    .polyCount = polyCount
  };
  return mdl;
}

Object object_new(Model *model, Color *palette, Vertex pos, f32 scale){
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

void object_move(Object *obj, Vertex dv){
  obj->pos.x += dv.x;
  obj->pos.y += dv.y;
  obj->pos.z += dv.z;
}

void object_render(Object *obj, SDL_Renderer *rend, Camera *cam, f32 cx, f32 cy){
  Vertex *vert = obj->model->vertex, clipped[3], unclipped[3];
  SDL_Vertex gradVert[3] = {0};
  u8 clipCount, unclipCount;

  for(size_t i = 0; i < obj->model->polyCount; ++i){
    clipCount = unclipCount = 0;
    Polygon polygon_new = obj->model->polygon[i];

    Vertex proj[4] = {
      vertex_onCamera(polygon_new.idx[0][vert], cam, obj->pos, obj->rot, obj->scale),
      vertex_onCamera(polygon_new.idx[1][vert], cam, obj->pos, obj->rot, obj->scale),
      vertex_onCamera(polygon_new.idx[2][vert], cam, obj->pos, obj->rot, obj->scale)
    };
    
    proj[0].x *= cam->fieldView, proj[0].y *= cam->fieldView;
    proj[1].x *= cam->fieldView, proj[1].y *= cam->fieldView;
    proj[2].x *= cam->fieldView, proj[2].y *= cam->fieldView;
    
    Color color = obj->palette[polygon_new.colorIndex];

    for(u8 j = 0; j < 3; ++j)
    if(proj[j].z <= cam->nearPlane) clipped[clipCount++] = proj[j];
    else unclipped[unclipCount++] = proj[j];

    bool extraVertex = false;
    switch(clipCount){
    case 0:
      for(u8 j = 0; j < 3; ++j)
        proj[j].x /= proj[j].z, proj[j].y /= proj[j].z;
    break;
    case 1:
      extraVertex = true;
      proj[0] = unclipped[0];
      proj[1] = unclipped[1];
      proj[2] = vertex_getClip(clipped[0], unclipped[0], cam->nearPlane);
      proj[3] = vertex_getClip(clipped[0], unclipped[1], cam->nearPlane);
      for(u8 j = 0; j < 2; ++j)
        proj[j].x /= proj[j].z, proj[j].y /= proj[j].z;
    break;
    case 2:{
      proj[0] = unclipped[0];
      proj[1] = vertex_getClip(clipped[0], unclipped[0], cam->nearPlane);
      proj[2] = vertex_getClip(clipped[1], unclipped[0], cam->nearPlane);
      proj[0].x /= proj[0].z, proj[0].y /= proj[0].z;
    }
    break;
    default:
      continue;
    }

    for(u8 j = 0; j < 3; ++j){
      gradVert[j].position = (SDL_FPoint){.x = proj[j].x + cx, .y = proj[j].y + cy};
      gradVert[j].color = (SDL_FColor){.a = color.a, .r = color.r, .g = color.g, .b = color.b};
    }
    if(extraVertex){
      SDL_RenderGeometry(rend, NULL, gradVert, 3, NULL, 0);
      gradVert[0].position = (SDL_FPoint){.x = proj[3].x + cx, .y = proj[3].y + cy};
      gradVert[0].color = (SDL_FColor){.a = color.a, .r = color.r, .g = color.g, .b = color.b};
    }
    SDL_RenderGeometry(rend, NULL, gradVert, 3, NULL, 0);
  }
}

int object_distCompare(void **ref, u32 *idx1, u32 *idx2){
  Camera *cam = ref[0];
  Object *objs = ref[1];
  Vertex diff = vertex_sub(objs[*idx1].pos, cam->pos);
  f32 distSq1 = vertex_dot(diff, diff);
  diff = vertex_sub(objs[*idx2].pos, cam->pos);
  f32 distSq2 = vertex_dot(diff, diff);
  return distSq1 > distSq2 ? -1 : 1;
}
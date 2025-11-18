#include "fold.h"
#include <SDL3_gfx/SDL3_gfxPrimitives.h>
#include <math.h>

Vertex vertex(f32 x, f32 y, f32 z){
  Vertex v = {
    .coord = {x, y, z}
  };
  return v;
}

Vertex vertex_rotX(Vertex *v, f32 s, f32 c){
  Vertex rot = {
    .x = v->x,
    .y = v->y * c - v->z * s,
    .z = v->y * s + v->z * c
  };
  return rot;
}

Vertex vertex_rotY(Vertex *v, f32 s, f32 c){
  Vertex rot = {
    .x = v->x * c - v->z * s,
    .y = v->y,
    .z = v->x * s + v->z * c
  };
  return rot;
}

Vertex vertex_rotZ(Vertex *v, f32 s, f32 c){
  Vertex rot = {
    .x = v->x * c - v->y * s,
    .y = v->x * s + v->y * c,
    .z = v->z
  };
  return rot;
}

Vertex vertex_getClip(Vertex clip, Vertex unclip, f32 z){
  Vertex v = {.z = z};
  f32
    dx = unclip.x - clip.x,
    dy = unclip.y - clip.y,
    dz = unclip.z - clip.z;
  if(dz == 0.f) dz = 1e-6f;
  v.x = dx / dz * (z - clip.z) + clip.x;
  v.y = dy / dz * (z - clip.z) + clip.y;
  return v;
}

int vertex_projectionCompare(Vertex *vert1, Vertex *vert2){
  return vert2->z - vert1->z;
}

Vertex vertex_add(Vertex a, Vertex b){
  return vertex(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vertex vertex_sub(Vertex a, Vertex b){
  return vertex(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vertex vertex_scalarMul(Vertex a, f32 scalar){
  return vertex(a.x * scalar, a.y * scalar, a.z * scalar);
}

Vertex vertex_scalarDiv(Vertex a, f32 scalar){
  return vertex(a.x / scalar, a.y / scalar, a.z / scalar);
}

f32 vertex_dot(Vertex a, Vertex b){
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Camera camera(Vertex v, f32 farPlane, f32 nearPlane, f32 fieldView){
  Camera cam = {
    .pos = v,
    .farPlane = farPlane,
    .nearPlane = nearPlane,
    .fieldView = fieldView
  };
  return cam;
}

void camera_rotate(Camera *cam, f32 dYaw, f32 dPitch){
  cam->yaw = SDL_fmodf(cam->yaw + dYaw, 2.f * SDL_PI_F);
  cam->pitch = SDL_min(SDL_max(cam->pitch + dPitch, SDL_PI_F / -2.f), SDL_PI_F / 2.f);
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

Vertex vertex_onCamera(Vertex *v, Camera *cam, Vertex offset, Vertex rot, f32 scale){
  Vertex dv = *v;
  f32 s, c;
  sincosf(rot.z, &s, &c);
  dv = vertex_rotZ(&dv, s, c);
  sincosf(rot.y, &s, &c);
  dv = vertex_rotY(&dv, s, c);
  sincosf(rot.x, &s, &c);
  dv = vertex_rotX(&dv, s, c);
  
  
  dv.x = dv.x * scale - cam->pos.x + offset.x,
  dv.y = dv.y * scale - cam->pos.y + offset.y,
  dv.z = dv.z * scale - cam->pos.z + offset.z;
  
  sincosf(cam->yaw, &s, &c);
  dv = vertex_rotY(&dv, s, c);

  sincosf(cam->pitch, &s, &c);
  return vertex_rotX(&dv, s, c);
}

Color color(f32 r, f32 g, f32 b, f32 a){
  Color c = {
    .rgba = {r, g, b, a}
  };
  return c;
}

Polygon polygon(u16 idx0, u16 idx1, u16 idx2, u16 colorIndex){
  Polygon p = {
    .idx = {idx0, idx1, idx2},
    .colorIndex = colorIndex
  };
  return p;
}

Model model(Vertex *vertex, size_t vertexCount, Polygon *polygon, size_t polyCount){
  Model mdl = {
    .vertex = vertex,
    .vertexCount = vertexCount,
    .polygon = polygon,
    .polyCount = polyCount
  };
  return mdl;
}

Object object(Model *model, Color *palette, Vertex pos, f32 scale){
  Object obj = {
    .model = model,
    .palette = palette,
    .pos = pos,
    .scale = scale
  };
  return obj;
}

void object_rotateX(Object *obj, f32 ang){
  obj->rot.x = SDL_fmodf(obj->rot.x + ang, 2.f * SDL_PI_F);
}

void object_rotateY(Object *obj, f32 ang){
  obj->rot.y = SDL_fmodf(obj->rot.y + ang, 2.f * SDL_PI_F);
}

void object_rotateZ(Object *obj, f32 ang){
  obj->rot.z = SDL_fmodf(obj->rot.z + ang, 2.f * SDL_PI_F);
}

void object_move(Object *obj, Vertex dv){
  obj->pos.x += dv.x;
  obj->pos.y += dv.y;
  obj->pos.z += dv.z;
}

#define ouch(x) (x / (x + 100.f))

f32 colorDamp(f32 x){
  x /= 100;
  x = SDL_expf(x);
  x *= x;
  x += 1;
  return 2.f / x;
}

void object_render(Object *obj, SDL_Renderer *rend, Camera *cam, f32 cx, f32 cy){
  Vertex *vert = obj->model->vertex, clipped[3], unclipped[3];
  SDL_Vertex gradVert[3] = {0};
  u8 clipCount, unclipCount;

  for(size_t i = 0; i < obj->model->polyCount; ++i){
    clipCount = unclipCount = 0;
    Polygon polygon = obj->model->polygon[i];
    Color color = obj->palette[polygon.colorIndex];
    Vertex proj[4] = {
      vertex_onCamera(vert + polygon.idx[0], cam, obj->pos, obj->rot, obj->scale),
      vertex_onCamera(vert + polygon.idx[1], cam, obj->pos, obj->rot, obj->scale),
      vertex_onCamera(vert + polygon.idx[2], cam, obj->pos, obj->rot, obj->scale)
    };

    proj[0].x *= cam->fieldView, proj[0].y *= cam->fieldView;
    proj[1].x *= cam->fieldView, proj[1].y *= cam->fieldView;
    proj[2].x *= cam->fieldView, proj[2].y *= cam->fieldView;

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

int object_distCompare(Camera *ref, Object *obj1, Object *obj2){
  Vertex diff = vertex_sub(obj1->pos, ref->pos);
  f32 distSq1 = vertex_dot(diff, diff);
  diff = vertex_sub(obj2->pos, ref->pos);
  f32 distSq2 = vertex_dot(diff, diff);
  return distSq1 > distSq2 ? -1 : 1;
}
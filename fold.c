#include "fold.h"
#include <math.h>

Vertex vertex(float x, float y, float z){
  Vertex v = {
    .coord = {x, y, z}
  };
  return v;
}

Vertex vertex_rotX(Vertex *v, float s, float c){
  Vertex rot = {
    .x = v->x,
    .y = v->y * c - v->z * s,
    .z = v->y * s + v->z * c
  };
  return rot;
}

Vertex vertex_rotY(Vertex *v, float s, float c){
  Vertex rot = {
    .x = v->x * c - v->z * s,
    .y = v->y,
    .z = v->x * s + v->z * c
  };
  return rot;
}

Vertex vertex_rotZ(Vertex *v, float s, float c){
  Vertex rot = {
    .x = v->x * c - v->y * s,
    .y = v->x * s + v->y * c,
    .z = v->z
  };
  return rot;
}

Vertex vertex_getClip(Vertex clip, Vertex unclip, float z){
  Vertex v = {.z = z};
  float
    dx = unclip.x - clip.x,
    dy = unclip.y - clip.y,
    dz = unclip.z - clip.z;
  if(dz == 0.f) dz = 1e-6f;
  v.x = dx / dz * (z - clip.z) + clip.x;
  v.y = dy / dz * (z - clip.z) + clip.y;
  return v;
}

Vertex vertex_add(Vertex a, Vertex b){
  return vertex(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vertex vertex_sub(Vertex a, Vertex b){
  return vertex(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vertex vertex_scalarMul(Vertex a, float scalar){
  return vertex(a.x * scalar, a.y * scalar, a.z * scalar);
}

Vertex vertex_scalarDiv(Vertex a, float scalar){
  return vertex(a.x / scalar, a.y / scalar, a.z / scalar);
}

float vertex_dot(Vertex a, Vertex b){
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

Camera camera(Vertex v, float farPlane, float nearPlane, float fieldView){
  Camera cam = {
    .pos = v,
    .farPlane = farPlane,
    .nearPlane = nearPlane,
    .fieldView = fieldView
  };
  return cam;
}

void camera_rotate(Camera *cam, float dYaw, float dPitch){
  cam->yaw = SDL_fmodf(cam->yaw + dYaw, 2.f * SDL_PI_F);
  cam->pitch = SDL_min(SDL_max(cam->pitch + dPitch, SDL_PI_F / -2.f), SDL_PI_F / 2.f);
}

void camera_moveAbs(Camera *cam, Vertex dPos){
  cam->pos = vertex_add(cam->pos, dPos);
}

void camera_moveRel(Camera *cam, Vertex dPos){
  float ps, pc, ys, yc;
  sincosf(cam->pitch, &ps, &pc);
  sincosf(cam->yaw, &ys, &yc);
  cam->pos.x += dPos.x * yc
                + dPos.y * -ys * ps
                + dPos.z * pc * ys;

    cam->pos.y += dPos.y * pc + dPos.z * ps;
    cam->pos.z += dPos.x * -ys + dPos.y * -yc * ps + dPos.z * pc * yc;
}

Vertex vertex_onCamera(Vertex *v, Camera *cam, Vertex offset, Vertex rot, float scale){
  Vertex dv = *v;
  float s, c;
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

Color color(Uint8 r, Uint8 g, Uint8 b, Uint8 a){
  Color c = {
    .rgba = {r, g, b, a}
  };
  return c;
}

Polygon polygon(Uint16 idx0, Uint16 idx1, Uint16 idx2, Uint16 colorIndex){
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

Object object(Model *model, Color *palette, Vertex pos, float scale){
  Object obj = {
    .model = model,
    .palette = palette,
    .pos = pos,
    .scale = scale
  };
  return obj;
}

void object_rotateX(Object *obj, float ang){
  obj->rot.x = SDL_fmodf(obj->rot.x + ang, 2.f * SDL_PI_F);
}

void object_rotateY(Object *obj, float ang){
  obj->rot.y = SDL_fmodf(obj->rot.y + ang, 2.f * SDL_PI_F);
}

void object_rotateZ(Object *obj, float ang){
  obj->rot.z = SDL_fmodf(obj->rot.z + ang, 2.f * SDL_PI_F);
}

void object_move(Object *obj, Vertex dv){
  obj->pos.x += dv.x;
  obj->pos.y += dv.y;
  obj->pos.z += dv.z;
}

void object_render(Object *obj, SDL_Renderer *rend, Camera *cam, float cx, float cy){
  Vertex *vert = obj->model->vertex, clipped[3], unclipped[3];
  Uint8 clipCount, unclipCount;
  float scale = obj->scale, s, c;

  for(size_t i = 0; i < obj->model->polyCount; ++i){
    clipCount = unclipCount = 0;
    Polygon polygon = obj->model->polygon[i];
    Color color = obj->palette[polygon.colorIndex];
    SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
    Vertex proj[3] = {
      vertex_onCamera(vert + polygon.idx[0], cam, obj->pos, obj->rot, scale),
      vertex_onCamera(vert + polygon.idx[1], cam, obj->pos, obj->rot, scale),
      vertex_onCamera(vert + polygon.idx[2], cam, obj->pos, obj->rot, scale)
    };

    proj[0].x *= cam->fieldView, proj[0].y *= cam->fieldView;
    proj[1].x *= cam->fieldView, proj[1].y *= cam->fieldView;
    proj[2].x *= cam->fieldView, proj[2].y *= cam->fieldView;

    if(proj[0].z <= cam->nearPlane) clipped[clipCount++] = proj[0];
    else unclipped[unclipCount++] = proj[0];
    if(proj[1].z <= cam->nearPlane) clipped[clipCount++] = proj[1];
    else unclipped[unclipCount++] = proj[1];
    if(proj[2].z <= cam->nearPlane) clipped[clipCount++] = proj[2];
    else unclipped[unclipCount++] = proj[2];

    switch(clipCount){
    case 0:{
      SDL_RenderLine(
        rend,
        proj[0].x / proj[0].z + cx, proj[0].y / proj[0].z + cy,
        proj[1].x / proj[1].z + cx, proj[1].y / proj[1].z + cy
      );
      SDL_RenderLine(
        rend,
        proj[1].x / proj[1].z + cx, proj[1].y / proj[1].z + cy,
        proj[2].x / proj[2].z + cx, proj[2].y / proj[2].z + cy
      );
      SDL_RenderLine(
        rend,
        proj[0].x / proj[0].z + cx, proj[0].y / proj[0].z + cy,
        proj[2].x / proj[2].z + cx, proj[2].y / proj[2].z + cy
      );
    }
    break;
    case 1:{
      Vertex
        v1 = vertex_getClip(clipped[0], unclipped[0], cam->nearPlane),
        v2 = vertex_getClip(clipped[0], unclipped[1], cam->nearPlane);
      SDL_RenderLine(
        rend,
        unclipped[0].x / unclipped[0].z + cx, unclipped[0].y / unclipped[0].z + cy,
        unclipped[1].x / unclipped[1].z + cx, unclipped[1].y / unclipped[1].z + cy
      );
      SDL_RenderLine(
        rend,
        unclipped[0].x / unclipped[0].z + cx, unclipped[0].y / unclipped[0].z + cy,
        v1.x / v1.z + cx, v1.y / v1.z + cy
      );
      SDL_RenderLine(
        rend,
        v1.x / v1.z + cx, v1.y / v1.z + cy,
        v2.x / v2.z + cx, v2.y / v2.z + cy
      );
      SDL_RenderLine(
        rend,
        v2.x / v2.z + cx, v2.y / v2.z + cy,
        unclipped[1].x / unclipped[1].z + cx, unclipped[1].y / unclipped[1].z + cy
      );
    }
    break;
    case 2:{
      Vertex
        v1 = vertex_getClip(clipped[0], unclipped[0], cam->nearPlane),
        v2 = vertex_getClip(clipped[1], unclipped[0], cam->nearPlane);
      SDL_RenderLine(
        rend,
        unclipped[0].x / unclipped[0].z + cx, unclipped[0].y / unclipped[0].z + cy,
        v1.x / v1.z + cx, v1.y / v1.z + cy
      );
      SDL_RenderLine(
        rend,
        v1.x / v1.z + cx, v1.y / v1.z + cy,
        v2.x / v2.z + cx, v2.y / v2.z + cy
      );
      SDL_RenderLine(
        rend,
        unclipped[0].x / unclipped[0].z + cx, unclipped[0].y / unclipped[0].z + cy,
        v2.x / v2.z + cx, v2.y / v2.z + cy
      );
    }
    break;
    }    
  }
}
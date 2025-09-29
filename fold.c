#include "fold.h"
#include <math.h>

Vertex vertex(float x, float y, float z){
  Vertex v = {
    .coord = {x, y, z}
  };
  return v;
}

void vertex_rotX(Vertex *v, float ang){
  Vertex cpy  = *v;
  float s, c;
  sincosf(ang, &s, &c);
  v->y = cpy.y * c - cpy.z * s;
  v->z = cpy.y * s + cpy.z * c;
}

void vertex_rotY(Vertex *v, float ang){
  Vertex cpy  = *v;
  float s, c;
  sincosf(ang, &s, &c);
  v->x = cpy.x * c - cpy.z * s;
  v->z = cpy.x * s + cpy.z * c;
}

void vertex_rotZ(Vertex *v, float ang){
  Vertex cpy  = *v;
  float s, c;
  sincosf(ang, &s, &c);
  v->x = cpy.x * c - cpy.y * s;
  v->y = cpy.x * s + cpy.y * c;
}

Camera camera(Vertex v, float nearPlane){
  Camera cam = {
    .pos = v,
    .nearPlane = nearPlane
  };
  return cam;
}

void camera_rotate(Camera *cam, float dYaw, float dPitch){
  cam->yaw = SDL_fmodf(cam->yaw + dYaw, SDL_PI_F);
  cam->pitch = SDL_min(SDL_max(cam->pitch + dPitch, SDL_PI_F / -2.f), SDL_PI_F / 2.f);
}

void camera_move(Camera *cam, Vertex dPos){
  cam->pos.x += dPos.x;
  cam->pos.y += dPos.y;
  cam->pos.z += dPos.z;
}

SDL_FPoint vertex_onCamera(Vertex *v, Camera *cam){
  Vertex dv = {
    .x = v->x * 120.f - cam->pos.x,
    .y = v->y * 120.f - cam->pos.y,
    .z = v->z - cam->pos.z
  };
  vertex_rotX(&dv, cam->yaw);
  vertex_rotY(&dv, cam->pitch);
  SDL_FPoint proj = {
    .x = dv.x / dv.z + 680.f,
    .y = dv.y / dv.z + 360.f
  };
  return proj;
}

Color color(Uint8 r, Uint8 g, Uint8 b, Uint8 a){
  Color c = {
    .rgba = {r, g, b, a}
  };
  return c;
}

Poly poly(Vertex *v1, Vertex *v2, Vertex *v3){
  Poly p = {
    .vert = {v1, v2, v3}
  };
  return p;
}

void poly_render(SDL_Renderer *rend, Camera *cam, Poly *poly, Color color){
  SDL_SetRenderDrawColor(rend, color.r, color.g, color.b, color.a);
  SDL_FPoint proj[3] = {
    vertex_onCamera(poly->vert[0], cam),
    vertex_onCamera(poly->vert[1], cam),
    vertex_onCamera(poly->vert[2], cam)
  };
  SDL_RenderLine(
    rend,
    proj[0].x,
    proj[0].y,
    proj[1].x,
    proj[1].y
  );
  SDL_RenderLine(
    rend,
    proj[1].x,
    proj[1].y,
    proj[2].x,
    proj[2].y
  );
  SDL_RenderLine(
    rend,
    proj[0].x,
    proj[0].y,
    proj[2].x,
    proj[2].y
  );
}
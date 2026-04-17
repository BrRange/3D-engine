#include <SDL3/SDL_stdinc.h>
#include "include/entity/camera.h"

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
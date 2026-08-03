#ifndef CAMERA_H_
#define CAMERA_H_

#include "dataType/quaternion.h"

struct Camera{
  Quaternion rot;
  Vec3 pos;
  f32 farPlane, nearPlane, fieldView;
  f32 yaw, pitch;
};
typedef struct Camera Camera;

Camera camera_new(const Vec3 v, f32 farPlane, f32 nearPlane, f32 fieldView);

Vec3 camera_viewVec3(const Camera *cam);

void camera_rotate(Camera *cam, f32 dYaw, f32 dPitch);

void camera_rotateUnbound(Camera *cam, f32 dYaw, f32 dPitch);

void camera_moveAbs(Camera *cam, const Vec3 dPos);

void camera_moveRel(Camera *cam, const Vec3 dPos);

Vec3 vec3_onCamera(Vec3 v, const Camera *restrict cam);

#endif
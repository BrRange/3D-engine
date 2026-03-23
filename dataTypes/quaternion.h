#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "vector.h"

typedef struct Quaternion{
  float r, x, y, z;
} Quaternion;

Quaternion quat_new(float angle, Vec3 normAxis);

Quaternion quat_compose(Quaternion quat, Quaternion val);

Quaternion quat_conjugate(Quaternion quat);

Quaternion vec3_quat(Vec3 point);

Vec3 quat_vec3(Quaternion quat);

Vec3 vec3_rotate(Vec3 point, Quaternion quat);

#endif
#ifndef DATATYPE_QUATERNION_H_
#define DATATYPE_QUATERNION_H_

#include "dataType/vector.h"

typedef struct Quaternion{
  f32 r, x, y, z;
} Quaternion;

Quaternion quat_new(f32 angle, Vec3 normAxis);

Quaternion quat_compose(Quaternion quat, Quaternion val);

Quaternion quat_conjugate(Quaternion quat);

Quaternion vec3_quat(Vec3 point);

Vec3 quat_vec3(Quaternion quat);

Vec3 vec3_rotate(Vec3 point, Quaternion quat);

#endif
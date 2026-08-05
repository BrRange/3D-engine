#ifndef DATATYPE_QUATERNION_H_
#define DATATYPE_QUATERNION_H_

#include "dataType/vector.h"

typedef Vec4 Quaternion;

Quaternion quat_new(f32 angle, Vec4 normAxis);

Quaternion quat_compose(Quaternion quat, Quaternion val);

Quaternion quat_conjugate(Quaternion quat);

Quaternion quat_add(Quaternion a, Quaternion b);

Quaternion quat_mul(Quaternion quat, f32 scale);

f32 quat_dot(Quaternion a, Quaternion b);

Quaternion quat_slerp(Quaternion src, Quaternion dest, f32 t);

Quaternion vec4_quat(Vec4 point);

Vec4 quat_vec4(Quaternion quat);

Vec4 vec4_rotate(Vec4 point, Quaternion quat);

#endif
#include "dataType/quaternion.h"
#include <math.h>

Quaternion quat_new(float angle, Vec3 normAxis){
  Quaternion quat;
  angle /= 2.f;
  sincosf(angle, &angle, &quat.r);
  quat.x = normAxis.x * angle;
  quat.y = normAxis.y * angle;
  quat.z = normAxis.z * angle;
  return quat;
}

Quaternion quat_compose(Quaternion quat, Quaternion val){
  Quaternion comp = {
    .r = quat.r * val.r - quat.x * val.x - quat.y * val.y - quat.z * val.z,
    .x = quat.r * val.x + quat.x * val.r + quat.y * val.z - quat.z * val.y,
    .y = quat.r * val.y - quat.x * val.z + quat.y * val.r + quat.z * val.x,
    .z = quat.r * val.z + quat.x * val.y - quat.y * val.x + quat.z * val.r,
  };
  return comp;
}

Quaternion quat_conjugate(Quaternion quat){
  quat.x *= -1.f;
  quat.y *= -1.f;
  quat.z *= -1.f;
  return quat;
}

Quaternion quat_add(Quaternion a, Quaternion b){
  a.r += b.r;
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;
  return a;
}

Quaternion quat_mul(Quaternion quat, f32 scale){
  quat.r *= scale;
  quat.x *= scale;
  quat.y *= scale;
  quat.z *= scale;
  return quat;
}

f32 quat_dot(Quaternion a, Quaternion b){
  return a.r * b.r + a.x * b.x + a.y * b.y + a.z * b.z;
}

Quaternion quat_slerp(Quaternion src, Quaternion dest, f32 t){
  f32 coef = quat_dot(src, dest);
  coef = 1.f - coef * coef;
  t = (t - t * t * t) / coef;
  src = quat_mul(src, 1.f - t);
  dest = quat_mul(dest, t);
  return quat_add(src, dest);
}

Quaternion vec3_quat(Vec3 point){
  return (Quaternion){0.f, point.x, point.y, point.z};
}

Vec3 quat_vec3(Quaternion quat){
  return (Vec3){quat.x, quat.y, quat.z};
}

Vec3 vec3_rotate(Vec3 point, Quaternion quat){
  Quaternion conjugate = quat_conjugate(quat);
  quat = quat_compose(quat, vec3_quat(point));
  quat = quat_compose(quat, conjugate);
  return quat_vec3(quat);
}
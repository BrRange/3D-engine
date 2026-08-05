#include "dataType/quaternion.h"
#include <math.h>

Quaternion quat_new(float angle, Vec4 normAxis){
  Quaternion quat;
  angle /= 2.f;
  sincosf(angle, &angle, &quat[0]);
  quat[1] = normAxis[0] * angle;
  quat[2] = normAxis[1] * angle;
  quat[3] = normAxis[2] * angle;
  return quat;
}

Quaternion quat_compose(Quaternion quat, Quaternion val){
  Quaternion comp = {
    quat[0] * val[0] - quat[1] * val[1] - quat[2] * val[2] - quat[3] * val[3],
    quat[0] * val[1] + quat[1] * val[0] + quat[2] * val[3] - quat[3] * val[2],
    quat[0] * val[2] - quat[1] * val[3] + quat[2] * val[0] + quat[3] * val[1],
    quat[0] * val[3] + quat[1] * val[2] - quat[2] * val[1] + quat[3] * val[0],
  };
  return comp;
}

Quaternion quat_conjugate(Quaternion quat){
  Vec4 v = {1, -1, -1, -1};
  return quat * v;
}

Quaternion quat_add(Quaternion a, Quaternion b){
  return a + b;
}

Quaternion quat_mul(Quaternion quat, f32 scale){
  return quat * scale;
}

f32 quat_dot(Quaternion a, Quaternion b){
  Quaternion q = a * b;
  return q[0] + q[1] + q[2] + q[3];
}

Quaternion quat_slerp(Quaternion src, Quaternion dest, f32 t){
  f32 coef = quat_dot(src, dest);
  coef = 1.f - coef * coef;
  t = (t - t * t * t) / coef;
  src = quat_mul(src, 1.f - t);
  dest = quat_mul(dest, t);
  return quat_add(src, dest);
}

Quaternion vec4_quat(Vec4 point){
  return (Quaternion){0.f, point[0], point[1], point[2]};
}

Vec4 quat_vec4(Quaternion quat){
  return (Vec4){quat[1], quat[2], quat[3]};
}

Vec4 vec4_rotate(Vec4 point, Quaternion quat){
  Quaternion conjugate = quat_conjugate(quat);
  quat = quat_compose(quat, vec4_quat(point));
  quat = quat_compose(quat, conjugate);
  return quat_vec4(quat);
}
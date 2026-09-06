#include "dataType/uniform.h"
#include <math.h>

struct Uniform uniform;

void uni_calcPerspective(Camera *cam, f32 aspect){
  f32 viewCoef = 1.f / tanf(cam->fieldView / 2);
  f32 frustum = cam->farPlane / (cam->farPlane - cam->nearPlane);
  uniform.perpective = (Mat4){
    aspect * viewCoef, 0, 0, 0,
    0, viewCoef, 0, 0,
    0, 0, frustum, 1,
    0, 0, 1, 0
  };
}
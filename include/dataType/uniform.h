#ifndef UNIFORM_H_
#define UNIFORM_H_

#include "dataType/rustydef.h"
#include "dataType/vector.h"
#include "dataType/canvas.h"
#include "entity/camera.h"

extern struct Uniform{
  f32 timer;
  Mat4 perpective;
} uniform;

void uni_calcPerspective(Camera *cam, f32 aspect);

#endif /* UNIFORM_H_ */
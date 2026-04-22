#ifndef LIGHT_SOURCE_H_
#define LIGHT_SOURCE_H_

#include "dataType/vector.h"

typedef enum LightSourceType{
  LightSourceType_Diffuse,
  LightSourceType_Radial,
  LightSourceType_Ambient
} LightSourceType;

typedef struct LightSource{
  int type;
  Vec3 intensity;
} LightSource;

typedef struct LightSource_Diffuse{
  LightSource base;
  Vec3 dir;
} LightSource_Diffuse;

LightSource_Diffuse lightSource_newDiffuse(Vec3 intensity, Vec3 dir);

Vec3 lightSource_diffuse_iluminate(LightSource_Diffuse *diffuse, Vec3 *vertex);

typedef struct LightSource_Radial{
  LightSource base;
  Vec3 *anchor;
} LightSource_Radial;

LightSource_Radial lightSource_newRadial(Vec3 intensity, Vec3 *anchor);

Vec3 lightSource_radial_iluminate(LightSource_Radial *radial, Vec3 *vertex);

typedef struct LightSource_Ambient{
  LightSource base;
} LightSource_Ambient;

LightSource_Ambient lightSource_newAmbient(Vec3 intensity);

Vec3 lightSource_ambient_iluminate(LightSource_Ambient *ambient);

typedef union LightSource_Packed{
  LightSource lightSource;
  LightSource_Diffuse diffuse;
  LightSource_Radial radial;
  LightSource_Ambient ambient;
} LightSource_Packed;

Vec3 lightSource_iluminate(LightSource *source, Vec3 *vertex);

#endif
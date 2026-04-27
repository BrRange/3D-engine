#include "entity/lightSource.h"
#include <SDL3/SDL_stdinc.h>

LightSource_Diffuse lightSource_newDiffuse(Vec3 intensity, Vec3 dir){
  LightSource_Diffuse diffuse = {
    .base.type = LightSourceType_Diffuse,
    .base.intensity = intensity,
    .dir = vec3_normal(dir)
  };
  return diffuse;
}

Vec3 lightSource_diffuse_iluminate(LightSource_Diffuse *diffuse, Vec3 *vertex){
  Vec3 cross = vec3_cross(vec3_sub(vertex[1], vertex[0]), vec3_sub(vertex[2], vertex[0]));
  cross = vec3_normal(cross);

  float alignment = vec3_dot(diffuse->dir, cross);
  return vec3_mul(diffuse->base.intensity, alignment);
}

LightSource_Radial lightSource_newRadial(Vec3 intensity, Vec3 *anchor){
  LightSource_Radial radial = {
    .base.type = LightSourceType_Radial,
    .base.intensity = intensity,
    .anchor = anchor
  };
  return radial;
}

Vec3 lightSource_radial_iluminate(LightSource_Radial *radial, Vec3 *vertex){
  return vec3_expand(0);
}

LightSource_Ambient lightSource_newAmbient(Vec3 intensity){
  LightSource_Ambient ambient = {
    .base.type = LightSourceType_Ambient,
    .base.intensity = intensity
  };
  return ambient;
}

Vec3 lightSource_ambient_iluminate(LightSource_Ambient *ambient){
  return ambient->base.intensity;
}

Vec3 lightSource_iluminate(LightSource *source, Vec3 *vertex){
  switch(source->type){
    case LightSourceType_Diffuse:{
      return lightSource_diffuse_iluminate((LightSource_Diffuse*)source, vertex);
    } break;
    case LightSourceType_Radial:{
      return lightSource_radial_iluminate((LightSource_Radial*)source, vertex);
    } break;
    case LightSourceType_Ambient:{
      return lightSource_ambient_iluminate((LightSource_Ambient*)source);
    } break;
  }
  return vec3_expand(0);
}
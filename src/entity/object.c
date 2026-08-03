#include "entity/object.h"

Polygon polygon_new(Vec3 normal, Vec2 *uv, u16 *idx){
  Polygon p = {
    .normal = normal,
    .idx = {idx[0], idx[1], idx[2]},
    .uv = {uv[0], uv[1], uv[2]}
  };
  return p;
}

Model model_new(Vec3 *vert, Polygon *polygon, size_t polyCount){
  Model mdl = {
    .vec3 = vert,
    .polygon = polygon,
    .polyCount = polyCount
  };
  return mdl;
}

Object object_new(Model *model, SDL_Surface *UVmap, const Vec3 pos, f32 scale){
  Object obj = {
    .model = model,
    .UVmap = UVmap,
    .pos = pos,
    .scale = scale,
    .rot = {1}
  };
  return obj;
}

void object_rotate(Object *obj, Quaternion quat){
  obj->rot = quat_compose(obj->rot, quat);
}

void object_move(Object *obj, const Vec3 dv){
  obj->pos += dv;
}

const Vec3 vec3_id = {1, 1, 1};

void object_render(Object *obj, Canvas *canv, Camera *cam, LightSource_Packed *sources){
  Vec3 *vert = obj->model->vec3, clipped[3], unclipped[3];
  u8 clipCount, unclipCount;

  for(size_t i = 0; i < obj->model->polyCount; ++i){
    clipCount = unclipCount = 0;
    Polygon polygon = obj->model->polygon[i];

    Vec3 vertex[3] = {polygon.idx[0][vert], polygon.idx[1][vert], polygon.idx[2][vert]};

    vertex[0] = vec3_rotate(vertex[0], obj->rot);
    vertex[1] = vec3_rotate(vertex[1], obj->rot);
    vertex[2] = vec3_rotate(vertex[2], obj->rot);

    vertex[0] *= obj->scale;
    vertex[1] *= obj->scale;
    vertex[2] *= obj->scale;

    vertex[0] += obj->pos;
    vertex[1] += obj->pos;
    vertex[2] += obj->pos;

    Vec3 lightPower = lightSource_iluminate(&sources[0].lightSource, vertex);
    lightPower[0] = SDL_clamp(lightPower[0], 0.f, 1.f);
    lightPower[1] = SDL_clamp(lightPower[1], 0.f, 1.f);
    lightPower[2] = SDL_clamp(lightPower[2], 0.f, 1.f);
    lightPower += lightSource_iluminate(&sources[1].lightSource, vertex);
    lightPower[0] = SDL_clamp(lightPower[0], 0.f, 1.f);
    lightPower[1] = SDL_clamp(lightPower[1], 0.f, 1.f);
    lightPower[2] = SDL_clamp(lightPower[2], 0.f, 1.f);

    Vec3 proj[4] = {
      vec3_onCamera(vertex[0], cam),
      vec3_onCamera(vertex[1], cam),
      vec3_onCamera(vertex[2], cam)
    };

    f32 aspecRatio = canv->w / canv->h;

    proj[0][0] *= cam->fieldView * aspecRatio * 500.f, proj[0][1] *= cam->fieldView / aspecRatio * 500.f;
    proj[1][0] *= cam->fieldView * aspecRatio * 500.f, proj[1][1] *= cam->fieldView / aspecRatio * 500.f;
    proj[2][0] *= cam->fieldView * aspecRatio * 500.f, proj[2][1] *= cam->fieldView / aspecRatio * 500.f;

    i32 lastClipped, lastUnclipped;

    for(i32 j = 0; j < 3; ++j)
    if(proj[j][2] <= cam->nearPlane){
      clipped[clipCount++] = proj[j];
      lastClipped = j;
    } else{
      unclipped[unclipCount++] = proj[j];
      lastUnclipped = j;
    }

    bool extraVec3 = false;
    Vec2 UV[6] = {
      polygon.uv[0],
      polygon.uv[1],
      polygon.uv[2]
    };
    switch(clipCount){
    case 0:{
      for(u8 j = 0; j < 3; ++j)
      proj[j][0] /= proj[j][2], proj[j][1] /= proj[j][2];
    } break;
    case 1:{
      extraVec3 = true;
      if(lastClipped == 1){
        proj[0] = unclipped[1];
        proj[1] = unclipped[0];
        proj[2] = vec3_getClip(clipped[0], unclipped[0], cam->nearPlane);
        proj[3] = vec3_getClip(clipped[0], unclipped[1], cam->nearPlane);
      } else{
        proj[0] = unclipped[0];
        proj[1] = unclipped[1];
        proj[2] = vec3_getClip(clipped[0], unclipped[1], cam->nearPlane);
        proj[3] = vec3_getClip(clipped[0], unclipped[0], cam->nearPlane);
      }
      proj[0][0] /= proj[0][2], proj[0][1] /= proj[0][2];
      proj[1][0] /= proj[1][2], proj[1][1] /= proj[1][2];
    } break;
    case 2:{
      proj[0] = unclipped[0];
      if(lastUnclipped == 1){
        proj[1] = vec3_getClip(clipped[1], unclipped[0], cam->nearPlane);
        proj[2] = vec3_getClip(clipped[0], unclipped[0], cam->nearPlane);
        UV[1] = vec2_mix(UV[0], UV[1], (clipped[1][2] - cam->nearPlane) / (clipped[1][2] - unclipped[0][2]));
        UV[2] = vec2_mix(UV[0], UV[2], (clipped[0][2] - cam->nearPlane) / (clipped[0][2] - unclipped[0][2]));
      } else{
        proj[1] = vec3_getClip(clipped[0], unclipped[0], cam->nearPlane);
        proj[2] = vec3_getClip(clipped[1], unclipped[0], cam->nearPlane);
        UV[1] = vec2_mix(UV[0], UV[1], (clipped[0][2] - cam->nearPlane) / (clipped[0][2] - unclipped[0][2]));
        UV[2] = vec2_mix(UV[0], UV[2], (clipped[1][2] - cam->nearPlane) / (clipped[1][2] - unclipped[0][2]));
      }
      proj[0][0] /= proj[0][2], proj[0][1] /= proj[0][2];
    } break;
    default:
      continue;
    }

    shader_pixel(canv, obj, UV, proj);

    if(extraVec3){
      proj[1] = proj[0];
      shader_pixel(canv, obj, UV + 3, proj + 1);
    }
  }
}
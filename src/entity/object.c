#include "entity/object.h"
#include "dataType/uniform.h"

Polygon polygon_new(Vec4 normal, Vec2 *uv, u16 *idx){
  Polygon p = {
    .normal = normal,
    .idx = {idx[0], idx[1], idx[2]},
    .uv = {uv[0], uv[1], uv[2]}
  };
  return p;
}

Model model_new(Vec4 *vert, Polygon *polygon, size_t polyCount){
  Model mdl = {
    .vert = vert,
    .polygon = polygon,
    .polyCount = polyCount
  };
  return mdl;
}

Object object_new(Model *model, SDL_Surface *UVmap, const Vec4 pos, f32 scale){
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

void object_move(Object *obj, const Vec4 dv){
  obj->pos += dv;
}

const Vec4 vec4_id = {1, 1, 1};

void object_render(Object *obj, Canvas *canv, Camera *cam){
  Vec4 *vert = obj->model->vert, clipped[3], unclipped[3];
  Vec2 clipUV[3], unclipUV[3];
  u8 clipCount, unclipCount;

  for(size_t i = 0; i < obj->model->polyCount; ++i){
    clipCount = unclipCount = 0;
    Polygon polygon = obj->model->polygon[i];

    Vec4 vertex[3] = {polygon.idx[0][vert], polygon.idx[1][vert], polygon.idx[2][vert]};

    vertex[0] = vec4_rotate(vertex[0], obj->rot);
    vertex[1] = vec4_rotate(vertex[1], obj->rot);
    vertex[2] = vec4_rotate(vertex[2], obj->rot);

    vertex[0] *= obj->scale;
    vertex[1] *= obj->scale;
    vertex[2] *= obj->scale;

    vertex[0] += obj->pos;
    vertex[1] += obj->pos;
    vertex[2] += obj->pos;

    Vec4 proj[4] = {
      vec4_onCamera(vertex[0], cam),
      vec4_onCamera(vertex[1], cam),
      vec4_onCamera(vertex[2], cam)
    };

    proj[0] = vec4_compose(uniform.perpective, proj[0]);
    proj[1] = vec4_compose(uniform.perpective, proj[1]);
    proj[2] = vec4_compose(uniform.perpective, proj[2]);

    for(int i = 0; i < 3; ++i){
      proj[i][0] *= canv->w;
      proj[i][1] *= canv->h;
      proj[i][3] = 1.f;
    }

    i32 lastClipped, lastUnclipped;

    for(i32 j = 0; j < 3; ++j)
    if(proj[j][2] <= cam->nearPlane){
      clipped[clipCount] = proj[j];
      clipUV[clipCount] = polygon.uv[j];
      ++clipCount;
      lastClipped = j;
    } else{
      unclipped[unclipCount] = proj[j];
      unclipUV[unclipCount] = polygon.uv[j];
      ++unclipCount;
      lastUnclipped = j;
    }

    bool extraVec3 = false;
    
    Vec2 UV[4];

    switch(clipCount){
      case 0:{
        for(u8 j = 0; j < 3; ++j){
          proj[j] /= proj[j][2];
          UV[j] = unclipUV[j];
        }
      } break;

      case 1:{
        extraVec3 = true;
        f32 coef0 = vec4_getClip(clipped[0], unclipped[0], cam->nearPlane),
        coef1 = vec4_getClip(clipped[0], unclipped[1], cam->nearPlane);
        if(lastClipped == 1){
          proj[1] = unclipped[0];
          proj[0] = unclipped[1];
          UV[1] = unclipUV[0];
          UV[0] = unclipUV[1];
          proj[2] = vec4_mix(unclipped[0], clipped[0], coef0);
          UV[2] = vec2_mix(unclipUV[0], clipUV[0], coef0);
          proj[3] = vec4_mix(unclipped[1], clipped[0], coef1);
          UV[3] = vec2_mix(unclipUV[1], clipUV[0], coef1);
        } else{
          proj[0] = unclipped[0];
          proj[1] = unclipped[1];
          UV[0] = unclipUV[0];
          UV[1] = unclipUV[1];
          proj[2] = vec4_mix(unclipped[1], clipped[0], coef1);
          UV[2] = vec2_mix(unclipUV[1], clipUV[0], coef1);
          proj[3] = vec4_mix(unclipped[0], clipped[0], coef0);
          UV[3] = vec2_mix(unclipUV[0], clipUV[0], coef0);
        }
        for(u8 j = 0; j < 4; ++j)
        proj[j] /= proj[j][2];
      } break;

      case 2:{
        proj[0] = unclipped[0];
        UV[0] = unclipUV[0];
        f32 coef0 = vec4_getClip(clipped[0], unclipped[0], cam->nearPlane),
        coef1 = vec4_getClip(clipped[1], unclipped[0], cam->nearPlane);
        if(lastUnclipped == 1){
          proj[1] = vec4_mix(unclipped[0], clipped[1], coef1);
          UV[1] = vec2_mix(unclipUV[0], clipUV[1], coef1);
          proj[2] = vec4_mix(unclipped[0], clipped[0], coef0);
          UV[2] = vec2_mix(unclipUV[0], clipUV[0], coef0);
        } else{
          proj[1] = vec4_mix(unclipped[0], clipped[0], coef0);
          UV[1] = vec2_mix(unclipUV[0], clipUV[0], coef0);
          proj[2] = vec4_mix(unclipped[0], clipped[1], coef1);
          UV[2] = vec2_mix(unclipUV[0], clipUV[1], coef1);
        }
        for(u8 j = 0; j < 3; ++j)
        proj[j] /= proj[j][2];
      } break;

      default:{
        continue;
      }
    }

    darray_append(&canv->vertex, proj);
    darray_append(&canv->uvCoord, UV);
    darray_appendPtr(&canv->uvSurface, obj->UVmap);

    if(extraVec3){
      proj[1] = proj[0];
      UV[1] = UV[0];
      darray_append(&canv->vertex, proj + 1);
      darray_append(&canv->uvCoord, UV + 1);
      darray_appendPtr(&canv->uvSurface, obj->UVmap);
    }
  }
}
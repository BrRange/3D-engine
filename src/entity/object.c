#include "entity/object.h"
#include "dataType/uniform.h"

Vertex vertex_new(Vec4 coord, Vec4 normal, Vec2 uv){
  Vertex v = {
    .coord = coord,
    .normal = normal,
    .uv = uv
  };
  return v;
}

Vertex vertex_mix(const Vertex a, const Vertex b, f32 t){
  Vertex v;
  v.coord = vec4_mix(a.coord, b.coord, t);
  v.normal = vec4_mix(a.normal, b.normal, t);
  v.uv = vec2_mix(a.uv, b.uv, t);
  return v;
}

Model model_new(Vertex *vert, Polygon *polygon, size_t polyCount){
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
  Vertex *vert = obj->model->vert, clipped[3], unclipped[3];
  u8 clipCount, unclipCount;

  for(size_t i = 0; i < obj->model->polyCount; ++i){
    clipCount = unclipCount = 0;

    u32 *poly = obj->model->polygon[i];

    Vertex vertex[4] = {obj->model->vert[poly[0]], obj->model->vert[poly[1]], obj->model->vert[poly[2]]};

    vertex[0].coord = vec4_rotate(vertex[0].coord, obj->rot);
    vertex[1].coord = vec4_rotate(vertex[1].coord, obj->rot);
    vertex[2].coord = vec4_rotate(vertex[2].coord, obj->rot);

    vertex[0].normal = vec4_rotate(vertex[0].normal, obj->rot);
    vertex[1].normal = vec4_rotate(vertex[1].normal, obj->rot);
    vertex[2].normal = vec4_rotate(vertex[2].normal, obj->rot);

    vertex[0].coord *= obj->scale;
    vertex[1].coord *= obj->scale;
    vertex[2].coord *= obj->scale;

    vertex[0].coord += obj->pos;
    vertex[1].coord += obj->pos;
    vertex[2].coord += obj->pos;

    vertex[0].coord = vec4_onCamera(vertex[0].coord, cam);
    vertex[1].coord = vec4_onCamera(vertex[1].coord, cam);
    vertex[2].coord = vec4_onCamera(vertex[2].coord, cam);

    vertex[0].coord = vec4_compose(uniform.perpective, vertex[0].coord);
    vertex[1].coord = vec4_compose(uniform.perpective, vertex[1].coord);
    vertex[2].coord = vec4_compose(uniform.perpective, vertex[2].coord);

    for(int i = 0; i < 3; ++i){
      vertex[i].coord[0] *= canv->w;
      vertex[i].coord[1] *= canv->h;
      vertex[i].coord[3] = 1.f;
    }

    i32 lastClipped, lastUnclipped;

    for(i32 j = 0; j < 3; ++j)
    if(vertex[j].coord[2] <= cam->nearPlane){
      clipped[clipCount] = vertex[j];
      ++clipCount;
      lastClipped = j;
    } else{
      unclipped[unclipCount] = vertex[j];
      ++unclipCount;
      lastUnclipped = j;
    }

    bool extraVec3 = false;

    switch(clipCount){
      case 0:{
        for(u8 j = 0; j < 3; ++j) vertex[j].coord /= vertex[j].coord[2];
      } break;

      case 1:{
        extraVec3 = true;
        f32 coef0 = vec4_getClip(clipped[0].coord, unclipped[0].coord, cam->nearPlane),
        coef1 = vec4_getClip(clipped[0].coord, unclipped[1].coord, cam->nearPlane);
        if(lastClipped == 1){
          vertex[1] = unclipped[0];
          vertex[0] = unclipped[1];
          vertex[2] = vertex_mix(unclipped[0], clipped[0], coef0);
          vertex[3] = vertex_mix(unclipped[1], clipped[0], coef1);
        } else{
          vertex[0] = unclipped[0];
          vertex[1] = unclipped[1];
          vertex[2] = vertex_mix(unclipped[1], clipped[0], coef1);
          vertex[3] = vertex_mix(unclipped[0], clipped[0], coef0);
        }
        for(u8 j = 0; j < 4; ++j)
        vertex[j].coord /= vertex[j].coord[2];
      } break;

      case 2:{
        vertex[0] = unclipped[0];
        f32 coef0 = vec4_getClip(clipped[0].coord, unclipped[0].coord, cam->nearPlane),
        coef1 = vec4_getClip(clipped[1].coord, unclipped[0].coord, cam->nearPlane);
        if(lastUnclipped == 1){
          vertex[1] = vertex_mix(unclipped[0], clipped[1], coef1);
          vertex[2] = vertex_mix(unclipped[0], clipped[0], coef0);
        } else{
          vertex[1] = vertex_mix(unclipped[0], clipped[0], coef0);
          vertex[2] = vertex_mix(unclipped[0], clipped[1], coef1);
        }
        for(u8 j = 0; j < 3; ++j)
        vertex[j].coord /= vertex[j].coord[2];
      } break;

      default:{
        continue;
      }
    }

    darray_append(&canv->vertices, vertex);
    darray_appendPtr(&canv->uvSurfaces, obj->UVmap);

    if(extraVec3){
      vertex[1] = vertex[0];
      darray_append(&canv->vertices, vertex + 1);
      darray_appendPtr(&canv->uvSurfaces, obj->UVmap);
    }
  }
}
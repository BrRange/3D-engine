#include "include/entity/object.h"

Polygon polygon_new(u16 idx0, u16 idx1, u16 idx2, u16 colorIndex){
  Polygon p = {
    .idx = {idx0, idx1, idx2},
    .colorIndex = colorIndex
  };
  return p;
}

Model model(Vec3 *vert, size_t vertCount, Polygon *polygon_new, size_t polyCount){
  Model mdl = {
    .vec3 = vert,
    .vec3Count = vertCount,
    .polygon = polygon_new,
    .polyCount = polyCount
  };
  return mdl;
}

Object object_new(Model *model, Color *palette, const Vec3 pos, f32 scale){
  Object obj = {
    .model = model,
    .palette = palette,
    .pos = pos,
    .scale = scale,
    .rot = {.r = 1}
  };
  return obj;
}

void object_rotate(Object *obj, Quaternion quat){
  obj->rot = quat_compose(obj->rot, quat);
}

void object_move(Object *obj, const Vec3 dv){
  obj->pos.x += dv.x;
  obj->pos.y += dv.y;
  obj->pos.z += dv.z;
}

const Vec3 vec3_id = {1, 1, 1};

void object_render(Object *obj, Canvas *canv, Camera *cam){
  Vec3 *vert = obj->model->vec3, clipped[3], unclipped[3];
  u8 clipCount, unclipCount;

  for(size_t i = 0; i < obj->model->polyCount; ++i){
    clipCount = unclipCount = 0;
    Polygon polygon_new = obj->model->polygon[i];

    Vec3 proj[4] = {
      vec3_onCamera(polygon_new.idx[0][vert], cam, obj->pos, obj->rot, obj->scale),
      vec3_onCamera(polygon_new.idx[1][vert], cam, obj->pos, obj->rot, obj->scale),
      vec3_onCamera(polygon_new.idx[2][vert], cam, obj->pos, obj->rot, obj->scale)
    };

    f32 aspecRatio = canv->w / canv->h;

    proj[0].x *= cam->fieldView * aspecRatio * 500.f, proj[0].y *= cam->fieldView / aspecRatio * 500.f;
    proj[1].x *= cam->fieldView * aspecRatio * 500.f, proj[1].y *= cam->fieldView / aspecRatio * 500.f;
    proj[2].x *= cam->fieldView * aspecRatio * 500.f, proj[2].y *= cam->fieldView / aspecRatio * 500.f;

    Color color = obj->palette[polygon_new.colorIndex];

    for(u8 j = 0; j < 3; ++j)
    if(proj[j].z <= cam->nearPlane) clipped[clipCount++] = proj[j];
    else unclipped[unclipCount++] = proj[j];

    bool extraVec3 = false;
    switch(clipCount){
    case 0:
      for(u8 j = 0; j < 3; ++j)
        proj[j].x /= proj[j].z, proj[j].y /= proj[j].z;
    break;
    case 1:
      extraVec3 = true;
      proj[0] = unclipped[0];
      proj[1] = unclipped[1];
      proj[2] = vec3_getClip(clipped[0], unclipped[1], cam->nearPlane);
      proj[3] = vec3_getClip(clipped[0], unclipped[0], cam->nearPlane);
      for(u8 j = 0; j < 2; ++j)
        proj[j].x /= proj[j].z, proj[j].y /= proj[j].z;
    break;
    case 2:{
      proj[0] = unclipped[0];
      proj[1] = vec3_getClip(clipped[0], unclipped[0], cam->nearPlane);
      proj[2] = vec3_getClip(clipped[1], unclipped[0], cam->nearPlane);
      proj[0].x /= proj[0].z, proj[0].y /= proj[0].z;
    }
    break;
    default:
      continue;
    }

    shader_pixel(canv, proj, color);

    if(extraVec3){
      proj[1] = proj[0];
      shader_pixel(canv, proj + 1, color);
    }
  }
}
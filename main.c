#include <SDL3/SDL.h>
#include "fold.c"
#include "player.c"
#include "sdlFrame.c"
#include "Models/crystalModel.c"
#include "Models/cubeModel.c"
#include "Models/icosphereModel.c"
#include "dataTypes/collider.c"
#include "dataTypes/vertex.c"
#include "dataTypes/quaternion.c"
#include "eventHandler.h"
#include "stdfcolor.h"

#define TIMEZONE -3

typedef struct CommonData{
  SDL_Time *deltaT;
  Camera *cam;
  MenuState *state;
  Object *objs;
  u32 *renderList;
  usz objCount;
  Collider *sphere, *pill;
} CommonData;

void tick(CommonData *data){
  f32 dt = *data->deltaT / 1000.f;

  f32 speed = KeyboardHandler_hasKey(data->state->keyboardH, SDLK_LSHIFT) ? dt * 600.f : dt * 60.f;

  Object *player = data->objs + 6;

  Quaternion time = quat_new(dt * M_PI / 30.f, vertex_new(0, 0, 1));
  object_rotate(data->objs, time);
  time = quat_new(dt * M_PI / 30.f / 60.f, vertex_new(0, 0, 1));
  object_rotate(data->objs + 1, time);
  time = quat_new(dt * M_PI / 30.f / 60.f / 12.f, vertex_new(0, 0, 1));
  object_rotate(data->objs + 2, time);

  SDL_FPoint mouseM = MouseHandler_getMovement(data->state->mouseH);

  camera_rotate(data->cam, mouseM.x * 0.01f, mouseM.y * 0.01f);
  Vertex cameraView = camera_viewVertex(data->cam), rotated;

  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_W)) object_move(player, vertex_scalarMul(cameraView, speed));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_S)) object_move(player, vertex_scalarMul(cameraView, -speed));
  rotated = vertex_new(-cameraView.z, 0, cameraView.x);
  rotated = vertex_scalarDiv(rotated, vertex_magnitude(rotated));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_A)) object_move(player, vertex_scalarMul(rotated, speed));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_D)) object_move(player, vertex_scalarMul(rotated, -speed));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_SPACE)) object_move(player, vertex_new(0, -speed, 0));
  if(KeyboardHandler_hasKey(data->state->keyboardH, SDLK_LCTRL)) object_move(player, vertex_new(0, speed, 0));

  cameraView = vertex_scalarMul(cameraView, 100);
  cameraView = vertex_sub(player->pos, cameraView);
  cameraView = vertex_sub(cameraView, data->cam->pos);
  camera_moveAbs(data->cam, vertex_scalarDiv(cameraView, 4));

  if(collider_collide(data->sphere, data->pill)) data->objs[6].scale = 10;
  else data->objs[6].scale = 5;

  void *sortData[2] = {data->cam, data->objs};

  SDL_qsort_r(data->renderList, data->objCount, sizeof *data->renderList, (void*)object_distCompare, sortData);
}

void render(SDL_Renderer *rend, CommonData *data){
  int w, h;
  SDL_GetRenderOutputSize(rend, &w, &h);
  f32 cx = w / 2, cy = h / 2;
  SDL_SetRenderDrawColor(rend, 55, 198, 255, 255);
  SDL_RenderClear(rend);

  for(usz i = 0; i < data->objCount; ++i)
  object_render(data->objs + data->renderList[i], rend, data->cam, cx, cy);
}

int main(){
  SDL_Window *win;
  SDL_Renderer *rend;

  SDL_CreateWindowAndRenderer("3D graphics", 100, 100, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED, &win, &rend);

  SDL_SetWindowRelativeMouseMode(win, true);
  SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);

  SDL_Time start, end = 0, dtime;
  start = SDL_GetTicks();

  KeyboardHandler kbHandler = {0};
  MouseHandler moHandler = {0};
  MenuState menu = MenuState_new(NULL, NULL, &kbHandler, &moHandler);
  Camera cam = camera(vertex_new(0, 0, 0), 200, 0.1f, 500);

  Color colors[] = {
    Color_Red,    // 0
    Color_Green,  // 1
    Color_Blue,   // 2
    Color_White,  // 3
    Color_Yellow, // 4
    Color_Cyan,   // 5
    Color_Magenta, // 6
    Color_Black // 7
  };

  Vertex zeCube_vert[] = {
    vertex_new(1, -1, -1),
    vertex_new(-1, -1, -1),
    vertex_new(-1, -1, 1),
    vertex_new(1, -1, 1),
    vertex_new(1, 1, -1),
    vertex_new(-1, 1, -1),
    vertex_new(-1, 1, 1),
    vertex_new(1, 1, 1)
  };

  Polygon zeCube_poly[] = {
    polygon_new(2, 1, 0, 0),
    polygon_new(3, 2, 0, 0),
    polygon_new(1, 4, 0, 0),
    polygon_new(5, 4, 1, 0),
    polygon_new(4, 3, 0, 0),
    polygon_new(4, 7, 3, 0),
    polygon_new(2, 5, 1, 0),
    polygon_new(6, 5, 2, 0),
    polygon_new(3, 6, 2, 0),
    polygon_new(7, 6, 3, 0),
    polygon_new(5, 6, 4, 0),
    polygon_new(6, 7, 4, 0)
  };

  Model zeCube_model = model(zeCube_vert, arrLen(zeCube_vert), zeCube_poly, arrLen(zeCube_poly));

  Vertex vert[][4] = {
    {vertex_new(.01, -1, 0), vertex_new(-.01, -1, 0), vertex_new(-.01, 0, 0), vertex_new(.01, 0, 0)},
    {vertex_new(.02, -1, 0), vertex_new(-.02, -1, 0), vertex_new(-.02, 0, 0), vertex_new(.02, 0, 0)},
    {vertex_new(.05, -1, 0), vertex_new(-.05, -1, 0), vertex_new(-.05, 0, 0), vertex_new(.05, 0, 0)},
  };
  Polygon poly[] = {polygon_new(0, 1, 2, 0), polygon_new(0, 2, 3, 0)};
  Model models[] = {
    {.vertex = vert[0], .vertexCount = arrLen(vert[0]), .polygon = poly, .polyCount = arrLen(poly)},
    {.vertex = vert[1], .vertexCount = arrLen(vert[1]), .polygon = poly, .polyCount = arrLen(poly)},
    {.vertex = vert[2], .vertexCount = arrLen(vert[2]), .polygon = poly, .polyCount = arrLen(poly)}
  };

  Vertex backdropVert[] = {
    vertex_new(0.5f, -sqrtf(3) / 2.f, 0),
    vertex_new(-0.5f, -sqrtf(3) / 2.f, 0),
    vertex_new(-1, 0, 0),
    vertex_new(-0.5f, sqrtf(3) / 2.f, 0),
    vertex_new(0.5f, sqrtf(3) / 2.f, 0),
    vertex_new(1, 0, 0)
  };
  Polygon backdropPoly[] = {
    polygon_new(0, 1, 5, 0),
    polygon_new(1, 2, 5, 0),
    polygon_new(2, 3, 4, 0),
    polygon_new(2, 4, 5, 0)
  };
  Model backdropModel = {.vertex = backdropVert, .vertexCount = arrLen(backdropVert), .polygon = backdropPoly, .polyCount = arrLen(backdropPoly)};

  Vertex thick_vert[] = {
    vertex_new(0.5f, -sqrtf(3) / 2.f, 0),
    vertex_new(-0.5f, -sqrtf(3) / 2.f, 0),
    vertex_new(-1, 0, 0),
    vertex_new(-0.5f, sqrtf(3) / 2.f, 0),
    vertex_new(0.5f, sqrtf(3) / 2.f, 0),
    vertex_new(1, 0, 0),
    vertex_new(0.5f, -sqrtf(3) / 2.f, 0.1),
    vertex_new(-0.5f, -sqrtf(3) / 2.f, 0.1),
    vertex_new(-1, 0, 0.1),
    vertex_new(-0.5f, sqrtf(3) / 2.f, 0.1),
    vertex_new(0.5f, sqrtf(3) / 2.f, 0.1),
    vertex_new(1, 0, 0.1)
  };

  Polygon thick_poly[] = {
    polygon_new(0, 1, 5, 0),
    polygon_new(1, 2, 5, 0),
    polygon_new(2, 3, 4, 0),
    polygon_new(2, 4, 5, 0),
    polygon_new(6, 7, 11, 0),
    polygon_new(7, 8, 11, 0),
    polygon_new(8, 9, 10, 0),
    polygon_new(8, 10, 11, 0),
    polygon_new(0, 6, 1, 0),
    polygon_new(7, 6, 1, 0),
    polygon_new(1, 7, 2, 0),
    polygon_new(8, 7, 2, 0),
    polygon_new(2, 8, 3, 0),
    polygon_new(9, 8, 3, 0),
    polygon_new(3, 9, 4, 0),
    polygon_new(10, 9, 4, 0),
    polygon_new(4, 10, 5, 0),
    polygon_new(11, 10, 5, 0),
    polygon_new(5, 11, 6, 0),
    polygon_new(5, 0, 6, 0)
  };

  Model thick_model = model(thick_vert, arrLen(thick_vert), thick_poly, arrLen(thick_poly));

  #define zeDim 10

  Object objs[(zeDim * zeDim) + 7] = {
    object_new(&models[0], colors + 7, vertex_new(-20, 5, 19.8), 100),
    object_new(&models[1], colors + 7, vertex_new(-20, 5, 19.8), 90),
    object_new(&models[2], colors + 7, vertex_new(-20, 5, 19.8), 40),
    object_new(&backdropModel, colors + 7, vertex_new(-20, 5, 19.8), 5),
    object_new(&backdropModel, colors + 3, vertex_new(-20, 5, 19.9), 120),
    object_new(&thick_model, colors + 2, vertex_new(-20, 5, 20), 130),
    object_new(&zeCube_model, colors, vertex_new(0, 0, 0), 5)
  };

  for(i32 i = 0; i < zeDim; ++i)
  for(i32 j = 0; j < zeDim; ++j){
    Object *objIns = objs + 7 + zeDim * i + j;
    *objIns = object_new(&zeCube_model, colors + ((i + j) % 8), vertex_new(i * 40, j * 40, 500), 20);
  }

  const u32 objLen = arrLen(objs);

  u32 renderList[objLen];
  for(u32 u = 0; u < objLen; ++u) renderList[u] = u;

  SDL_GetCurrentTime(&dtime);
  float clockAngle = dtime / 1e9f;
  Quaternion time = quat_new(clockAngle * M_PI / 30.f, vertex_new(0, 0, 1));
  object_rotate(objs + 0, time);
  clockAngle /= 60.f;
  time = quat_new(clockAngle * M_PI / 30.f, vertex_new(0, 0, 1));
  object_rotate(objs + 1, time);
  clockAngle /= 60.f;
  clockAngle += TIMEZONE;
  time = quat_new(clockAngle * M_PI / 6.f, vertex_new(0, 0, 1));
  object_rotate(objs + 2, time);

  Collider_Sphere playerColl = collider_newSphere(objs + 6, vertex_new(0, 0, 0), objs[6].scale);
  Collider_Pill secondColl = collider_newPill(objs + 3, vertex_new(0, -100, 0), vertex_new(0, 100, 0), 5);

  CommonData data = {
    .deltaT = &dtime,
    .cam = &cam,
    .state = &menu,
    .objs = objs,
    .renderList = renderList,
    .objCount = objLen,
    .sphere = (Collider*)&playerColl,
    .pill = (Collider*)&secondColl
  };

  SDL_Event eve = {0};
  while(eve.type != SDL_EVENT_QUIT){
    dtime = start - end;
    if(dtime >= 16){
      end = start;
      handleEvents(&eve, &menu);

      tick(&data);
      render(rend, &data);

      SDL_RenderPresent(rend);
    }
    start = SDL_GetTicks();
    SDL_Delay(1);
  }

  SDL_DestroyWindow(win);
  SDL_DestroyRenderer(rend);
  SDL_assert((SDL_Log("Normal execution"), 0));
}
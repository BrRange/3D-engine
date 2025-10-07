#include "SDL3/SDL.h"

union Vertex{
  float coord[3];
  struct{
    float x, y, z;
  };
};
typedef union Vertex Vertex;

Vertex vertex(float x, float y, float z);

Vertex vertex_rotX(Vertex *v, float s, float c);

Vertex vertex_rotY(Vertex *v, float s, float c);

Vertex vertex_rotZ(Vertex *v, float s, float c);

Vertex vertex_getClip(Vertex clip, Vertex unclip, float z);

struct Camera{
  Vertex pos;
  float farPlane, nearPlane, fieldView;
  float yaw, pitch;
};
typedef struct Camera Camera;

Camera camera(Vertex v, float farPlane, float nearPlane, float fieldView);

void camera_rotate(Camera *cam, float dYaw, float dPitch);

void camera_move(Camera *cam, Vertex dPos);

Vertex vertex_onCamera(Vertex *v, Camera *cam, Vertex offset, Vertex rot, float scale);

union Color{
  Uint8 rgba[4];
  struct{
    Uint8 r, g, b, a;
  };
};
typedef union Color Color;

Color color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

struct Poly{
  Uint16 idx[3];
  Uint16 colorIndex;
};
typedef struct Poly Poly;

Poly poly(Uint16 idx0, Uint16 idx1, Uint16 idx2, Uint16 colorIndex);

struct Model{
  Vertex *vertex;
  size_t vertexCount;
  Poly *poly;
  size_t polyCount;
};
typedef struct Model Model;

Model model(Vertex *vertex, size_t vertexCount, Poly *poly, size_t polyCount);

struct Object{
  Model *model;
  Color *palette;
  Vertex pos, rot;
  float scale;
};
typedef struct Object Object;

Object object(Model *model, Color *palette, Vertex rCenter, float scale);

void object_rotateX(Object *obj, float ang);

void object_rotateY(Object *obj, float ang);

void object_rotateZ(Object *obj, float ang);

void object_move(Object *obj, Vertex dv);

void object_render(Object *obj, SDL_Renderer *rend, Camera *cam, float cx, float cy);
#include "SDL3/SDL.h"

union Vertex{
  float coord[3];
  struct{
    float x, y, z;
  };
};
typedef union Vertex Vertex;

Vertex vertex(float x, float y, float z);
void vertex_rotX(Vertex *v, float ang);
void vertex_rotY(Vertex *v, float ang);
void vertex_rotZ(Vertex *v, float ang);

struct Camera{
  Vertex pos;
  float nearPlane;
  float pitch, yaw;
};
typedef struct Camera Camera;

Camera camera(Vertex v, float nearPlane);
void camera_rotate(Camera *cam, float dYaw, float dPitch);
void camera_move(Camera *cam, Vertex dPos);

SDL_FPoint vertex_onCamera(Vertex *v, Camera *cam);

union Color{
  Uint8 rgba[4];
  struct{
    Uint8 r, g, b, a;
  };
};
typedef union Color Color;

Color color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

struct Poly{
  Vertex *vert[3];
};
typedef struct Poly Poly;

Poly poly(Vertex *v1, Vertex *v2, Vertex *v3);
void poly_render(SDL_Renderer *rend, Camera *cam, Poly *poly, Color color);
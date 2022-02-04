#include "LoadPlane.h"
#include "OpenGL/OGLGraphics.h"
namespace AA {

static unsigned int vao_to_the_plane = 0;
static bool plane_loaded = false;

unsigned int load_plane() {

  if (plane_loaded) {
    return vao_to_the_plane;
  }
  const float SIZE = 1.f;
  const float planeVertices[] = {
    // positions
    -SIZE, 0, SIZE,
    SIZE, 0, SIZE,
    -SIZE, 0, -SIZE,
    SIZE, 0, -SIZE
  };

  unsigned int faces[] = {   //indices
    1,2,0,1,3,2
  };

  vao_to_the_plane = OGLGraphics::Upload3DPositionsMesh(planeVertices, sizeof(planeVertices) / sizeof(planeVertices[0]), faces, sizeof(faces) / sizeof(faces[0]));
  plane_loaded = true;
  return vao_to_the_plane;
}


}
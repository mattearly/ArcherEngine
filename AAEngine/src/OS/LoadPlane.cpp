#include "LoadPlane.h"
#include "OpenGL/OGLGraphics.h"
namespace AA {

static unsigned int vao_to_the_plane = 0;

unsigned int load_plane() {

  if (vao_to_the_plane != 0) {
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
  return vao_to_the_plane;
}

void unload_plane() {
  if (vao_to_the_plane != 0) {
    OGLGraphics::DeleteMesh(vao_to_the_plane);
    vao_to_the_plane = 0;
  }
}

}
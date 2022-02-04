#include "LoadCube.h"
#include "OpenGL/OGLGraphics.h"
namespace AA {
static unsigned int vao_to_the_cube = 0;
static bool cube_loaded = false;

// returns the vao to a -1 by 1 cube
unsigned int load_cube() {
  if (cube_loaded) {
    return vao_to_the_cube;
  }
  const float cubeVertices[] = {
    // front
    -1.0, -1.0,  1.0,
    1.0, -1.0,  1.0,
    1.0,  1.0,  1.0,
    -1.0,  1.0,  1.0,
    // back
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    1.0,  1.0, -1.0,
    -1.0,  1.0, -1.0
  };

  const unsigned int faces[] = {
    // front
    0, 1, 2,
    2, 3, 0,
    // right
    1, 5, 6,
    6, 2, 1,
    // back
    7, 6, 5,
    5, 4, 7,
    // left
    4, 0, 3,
    3, 7, 4,
    // bottom
    4, 5, 1,
    1, 0, 4,
    // top
    3, 2, 6,
    6, 7, 3 };

  vao_to_the_cube = OGLGraphics::Upload3DPositionsMesh(cubeVertices, sizeof(cubeVertices) / sizeof(cubeVertices[0]), faces, sizeof(faces) / sizeof(faces[0]));

  cube_loaded = true;
  return vao_to_the_cube;
}

}  // end namespace AA
#include "PrimativeMaker.h"
#include "Graphics.h"
namespace AA {

static unsigned int vao_to_the_cube = 0;
// creates and returns vao to a -1 by 1 cube
unsigned int PrimativeMaker::load_cube() {
  if (vao_to_the_cube != 0) {
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

  vao_to_the_cube = OpenGL::GetGL()->Upload3DPositionsMesh(cubeVertices, sizeof(cubeVertices) / sizeof(cubeVertices[0]), faces, sizeof(faces) / sizeof(faces[0]));

  return vao_to_the_cube;
}
void PrimativeMaker::unload_cube() {
  if (vao_to_the_cube != 0) {
    OpenGL::GetGL()->DeleteMesh(1u, vao_to_the_cube);
    vao_to_the_cube = 0;
  }
}

static unsigned int vao_to_the_plane = 0;
// creates and returns vao to a -1 by 1 plane
unsigned int PrimativeMaker::load_plane() {

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

  vao_to_the_plane = OpenGL::GetGL()->Upload3DPositionsMesh(planeVertices, sizeof(planeVertices) / sizeof(planeVertices[0]), faces, sizeof(faces) / sizeof(faces[0]));
  return vao_to_the_plane;
}
void PrimativeMaker::unload_plane() {
  if (vao_to_the_plane != 0) {
    OpenGL::GetGL()->DeleteMesh(1u, vao_to_the_plane);
    vao_to_the_plane = 0;
  }
}

static unsigned int vao_to_the_cone = 0;
// creates and returns vao to a botched cone
unsigned int PrimativeMaker::load_cone() {

  // todo: fix?
  unsigned int faces[] = {   //indices
    1,1,1,33,2,1,2,3,1,
2,3,2,33,2,2,3,4,2,
3,4,3,33,2,3,4,5,3,
4,5,4,33,2,4,5,6,4,
5,6,5,33,2,5,6,7,5,
6,7,6,33,2,6,7,8,6,
7,8,7,33,2,7,8,9,7,
8,9,8,33,2,8,9,10,8,
9,10,9,33,2,9,10,11,9,
10,11,10,33,2,10,11,12,10,
11,12,11,33,2,11,12,13,11,
12,13,12,33,2,12,13,14,12,
13,14,13,33,2,13,14,15,13,
14,15,14,33,2,14,15,16,14,
15,16,15,33,2,15,16,17,15,
16,17,16,33,2,16,17,18,16,
17,18,17,33,2,17,18,19,17,
18,19,18,33,2,18,19,20,18,
19,20,19,33,2,19,20,21,19,
20,21,20,33,2,20,21,22,20,
21,22,21,33,2,21,22,23,21,
22,23,22,33,2,22,23,24,22,
23,24,23,33,2,23,24,25,23,
24,25,24,33,2,24,25,26,24,
25,26,25,33,2,25,26,27,25,
26,27,26,33,2,26,27,28,26,
27,28,27,33,2,27,28,29,27,
28,29,28,33,2,28,29,30,28,
29,30,29,33,2,29,30,31,29,
30,31,30,33,2,30,31,32,30,
1,33,31,2,34,31,3,35,31,4,36,31,5,37,31,6,38,31,7,39,31,8,40,31,9,41,31,10,42,31,11,43,31,12,44,31,13,45,31,14,46,31,15,47,31,16,48,31,17,49,31,18,50,31,19,51,31,20,52,31,21,53,31,22,54,31,23,55,31,24,56,31,25,57,31,26,58,31,27,59,31,28,60,31,29,61,31,30,62,31,31,63,31,32,64,31,
31,32,32,33,2,32,32,65,32,
32,65,33,33,2,33,1,1,33


  };

 unsigned int out_num_elements = sizeof(faces) / sizeof(faces[0]);

  if (vao_to_the_cone == 0) {
    // needs loaded
    const float coneVertices[] = {
      // positions
1.000000f,0.000007f,-1.000000f,
0.999999f,0.195098f,-0.980785f,
0.999997f,0.382691f,-0.923880f,
0.999996f,0.555578f,-0.831470f,
0.999995f,0.707114f,-0.707107f,
0.999994f,0.831477f,-0.555570f,
0.999993f,0.923887f,-0.382683f,
0.999993f,0.980793f,-0.195090f,
0.999993f,1.000007f,0.000000f,
0.999993f,0.980793f,0.195090f,
0.999993f,0.923887f,0.382683f,
0.999994f,0.831477f,0.555570f,
0.999995f,0.707114f,0.707107f,
0.999996f,0.555578f,0.831470f,
0.999997f,0.382691f,0.923880f,
0.999999f,0.195098f,0.980785f,
1.000000f,0.000007f,1.000000f,
1.000001f,-0.195083f,0.980785f,
1.000003f,-0.382676f,0.923880f,
1.000004f,-0.555563f,0.831470f,
1.000005f,-0.707099f,0.707107f,
1.000006f,-0.831462f,0.555570f,
1.000007f,-0.923872f,0.382684f,
1.000007f,-0.980778f,0.195090f,
1.000007f,-0.999993f,-0.000000f,
1.000007f,-0.980778f,-0.195090f,
1.000007f,-0.923872f,-0.382684f,
1.000006f,-0.831462f,-0.555570f,
1.000005f,-0.707099f,-0.707107f,
1.000004f,-0.555563f,-0.831470f,
1.000003f,-0.382676f,-0.923880f,
1.000001f,-0.195083f,-0.980785f,
-1.000000f,-0.000007f,0.000000f

    };
    unsigned int num_verts = sizeof(coneVertices) / sizeof(coneVertices[0]);
    vao_to_the_cone = OpenGL::GetGL()->Upload3DPositionsMesh(coneVertices, sizeof(coneVertices) / sizeof(coneVertices[0]), faces, sizeof(faces) / sizeof(faces[0]));
  }

  return vao_to_the_cone;
}
void PrimativeMaker::unload_cone() {
  if (vao_to_the_cone != 0) {
    OpenGL::GetGL()->DeleteMesh(1u, vao_to_the_cone);
    vao_to_the_cone = 0;
  }
}

//utility
void PrimativeMaker::unload_all() {
  unload_cone();
  unload_cube();
  unload_plane();
}

}
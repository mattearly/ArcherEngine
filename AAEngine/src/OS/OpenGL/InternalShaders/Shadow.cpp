#include "Shadow.h"
#include <string>
namespace AA::InternalShaders {

static OGLShader* SHADOWSHADER = NULL;  // setup once

//todo: optimization - uniform buffers https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
void Shadow::Init() {
  if (SHADOWSHADER)
    return;

  const std::string VERT_CODE =
    R"(
#version 430 core
layout (location = 0) in vec3 inPos;

uniform mat4 u_light_space_matrix;
uniform mat4 u_model_matrix;

void main()
{
    gl_Position = u_light_space_matrix * u_model_matrix * vec4(inPos, 1.0);
}
)";

  const std::string FRAG_CODE =
    R"(
#version 430 core

void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
}
)";

  SHADOWSHADER = new OGLShader(VERT_CODE.c_str(), FRAG_CODE.c_str());
}

OGLShader* Shadow::Get() {
  if (!SHADOWSHADER) {
    Init();
  } else {
    SHADOWSHADER->Use();
  }
  return SHADOWSHADER;
}

void Shadow::Shutdown() {
  if (SHADOWSHADER) {
    delete SHADOWSHADER;
    SHADOWSHADER = nullptr;
  }
}
bool Shadow::IsActive() {
  return SHADOWSHADER;
}
}
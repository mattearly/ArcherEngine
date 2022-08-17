#include "Skycube.h"
#include <string>
namespace AA::InternalShaders {

static OGLShader* SKYCUBESHADER = NULL;  // setup once

//todo: optimization - uniform buffers https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
void Skycube::Init() {
  if (SKYCUBESHADER)
    return;

  const std::string VERT_CODE =
    R"(
#version 430 core
layout(location = 0) in vec3 inPos;
out vec3 TexCoords;
uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
void main(){
  TexCoords = inPos;
  vec4 pos = u_projection_matrix * u_view_matrix * vec4(inPos, 1.0);
  gl_Position = pos.xyww;
}
)";

  const std::string FRAG_CODE = 
    R"(
#version 430 core
in vec3 TexCoords;
out vec4 FragColor;
uniform samplerCube u_skybox;
void main() {
  FragColor = texture(u_skybox, TexCoords);
}
)";

  SKYCUBESHADER = new OGLShader(VERT_CODE.c_str(), FRAG_CODE.c_str());
}

OGLShader* Skycube::Get() {
  if (!SKYCUBESHADER) {
    Init();
  } else {
    SKYCUBESHADER->Use();
  }
  return SKYCUBESHADER;
}

void Skycube::Shutdown() {
  if (SKYCUBESHADER) {
    delete SKYCUBESHADER;
    SKYCUBESHADER = nullptr;
  }
}
bool Skycube::IsActive() {
  return SKYCUBESHADER;
}
}
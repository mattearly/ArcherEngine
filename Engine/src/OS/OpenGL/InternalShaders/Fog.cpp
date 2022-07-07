#include "Fog.h"
#include <string>
namespace AA::InternalShaders {

static OGLShader* FOGSHADER = NULL;  // setup once

void Fog::Init() {
  if (FOGSHADER)
    return;

  const std::string VERT_CODE =
    R"(
#version 430 core
layout (location = 0) in vec3 inPos;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;
uniform mat4 u_model_matrix;
void main() {
    gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vec4(inPos, 1.0);
}
)";

  const std::string FRAG_CODE =
    R"(
#version 430 core
out vec4 out_Color;
void main() {             
    out_Color = vec4(1.0,1.0,1.0,1.0);
}
)";

  FOGSHADER = new OGLShader(VERT_CODE.c_str(), FRAG_CODE.c_str());
}

OGLShader* Fog::Get() {
  if (!FOGSHADER) {
    Init();
  } else {
    FOGSHADER->Use();
  }
  return FOGSHADER;
}

void Fog::Shutdown() {
  if (FOGSHADER) {
    delete FOGSHADER;
    FOGSHADER = nullptr;
  }
}

bool Fog::IsActive() {
  return FOGSHADER;
}

} // end namespace AA::InternalShaders
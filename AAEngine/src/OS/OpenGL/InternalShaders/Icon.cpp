#include "Icon.h"
#include <string>
namespace AA::InternalShaders {

static OGLShader* ICONSHADER = NULL;  // setup once

void Icon::Init() {
  if (ICONSHADER)
    return;

  const std::string VERT_CODE =
    R"(#version 430 core
layout (location = 0) in vec3 inPos;
uniform mat4 u_view_matrix;
uniform mat4 u_projection_matrix;
uniform mat4 u_model_matrix;
void main() {
    gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vec4(inPos, 1.0);
})";

  const std::string FRAG_CODE =
    R"(#version 430 core
out vec4 out_Color;
void main() {             
    out_Color = vec4(1.0,1.0,1.0,1.0);
})";

  ICONSHADER = new OGLShader(VERT_CODE.c_str(), FRAG_CODE.c_str());
}

OGLShader* Icon::Get() {
  if (!ICONSHADER) {
    Init();
  } else {
    ICONSHADER->Use();
  }
  return ICONSHADER;
}

void Icon::Shutdown() {
  if (ICONSHADER) {
    delete ICONSHADER;
    ICONSHADER = nullptr;
  }
}

bool Icon::IsActive() {
  return ICONSHADER;
}

} // end namespace AA::InternalShaders
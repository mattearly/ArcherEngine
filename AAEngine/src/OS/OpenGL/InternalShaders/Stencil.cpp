#include "Stencil.h"
#include <string>
namespace AA::InternalShaders {

static OGLShader* STENCILSHADER = nullptr;

//todo: optimization - uniform buffers https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
void Stencil::Init() {
  if (STENCILSHADER)
    return;

  const std::string VERT_CODE = 
    R"(#version 430 core
layout(location=0)in vec3 inPos;
layout(location=2)in vec3 inNorm;
layout(location=3)in ivec4 inBoneIds;
layout(location=4)in vec4 inWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;

uniform mat4 u_final_bone_mats[MAX_BONES];
uniform int u_stencil_with_normals;
uniform float u_stencil_scale;
uniform int u_is_animating;

void main(){
  vec4 totalPosition = vec4(0.0);
  if (u_is_animating > 0) {
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
      if(inBoneIds[i] == -1) continue;
      if(inBoneIds[i] >= MAX_BONES) {
          totalPosition = vec4(inPos, 1.0f);
          break;
      }
      vec4 localPosition = u_final_bone_mats[inBoneIds[i]] * vec4(inPos,1.0);
      totalPosition += localPosition * inWeights[i];
    }
  } else {  // Not Animating
    totalPosition = vec4(inPos, 1.0);
  }
  mat4 viewMatrix = u_view_matrix * u_model_matrix;
  if (u_stencil_with_normals > 0) {
    gl_Position = u_projection_matrix * viewMatrix * vec4(totalPosition.xyz + inNorm * (u_stencil_scale - 1.0), 1.0);
  } else {
    gl_Position = u_projection_matrix * viewMatrix * totalPosition;
  }
})";

  const std::string FRAG_CODE = R"(#version 430
out vec4 FragColor;
uniform vec3 u_stencil_color;
void main() {
  FragColor = vec4(u_stencil_color, 1.0);
})";

  STENCILSHADER = new OGLShader(VERT_CODE.c_str(), FRAG_CODE.c_str());
}

OGLShader* Stencil::Get() {
  if (!STENCILSHADER) {
    Init();
  } else {
    STENCILSHADER->Use();
  }
  return STENCILSHADER;
}

void Stencil::Shutdown() {
  if (STENCILSHADER) {
    delete STENCILSHADER;
    STENCILSHADER = nullptr;
  }
}
bool Stencil::IsActive() {
  return STENCILSHADER;
}
}
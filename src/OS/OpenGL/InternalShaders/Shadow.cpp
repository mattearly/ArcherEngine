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
layout (location = 3) in ivec4 inBoneIds;
layout (location = 4) in vec4 inWeights;

uniform mat4 u_light_space_matrix;
uniform mat4 u_model_matrix;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 u_final_bone_mats[MAX_BONES];
uniform int u_is_animating;

void main()
{
  vec4 totalPosition = vec4(0.0);

  if (u_is_animating > 0) {
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
      if(inBoneIds[i] == -1) continue;
        if(inBoneIds[i] >= MAX_BONES) {
          totalPosition = vec4(inPos, 1.0f);
          break;
        }
      vec4 localPosition = u_final_bone_mats[inBoneIds[i]] * vec4(inPos, 1.0);
      totalPosition += localPosition * inWeights[i];
     }
  } else {  // Not Animating
    //vs_out.Pos = (u_model_matrix * vec4(inPos, 1.0)).xyz;
    totalPosition = vec4(inPos, 1.0);
  }

  gl_Position = u_light_space_matrix * u_model_matrix * totalPosition;
}
)";

  const std::string FRAG_CODE =
    R"(
#version 430 core
void main() {}
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

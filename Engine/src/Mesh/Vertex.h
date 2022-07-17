#pragma once
#include <glm/glm.hpp>
namespace AA {

struct UnlitVertex {
  UnlitVertex(glm::vec3 pos, glm::vec2 uv);
  glm::vec3 Position{};
  glm::vec2 TexCoords{};
};

struct LitVertex : public UnlitVertex {
  LitVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal);
  glm::vec3 Normal{};
};

struct TanVertex :public LitVertex {
  TanVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal, glm::vec3 tan, glm::vec3 bitan);
  glm::vec3 Tangent{};
  glm::vec3 BiTangent{};
};

// These must also match on the shader code when processing bones
//constexpr auto MAX_BONES = 100;  // todo: we don't seem to be using this but we should, somewhere
constexpr auto MAX_BONE_INFLUENCE = 4;

struct AnimVertex : public LitVertex {
  AnimVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal);
  int m_BoneIDs[MAX_BONE_INFLUENCE];   //> bone indexes which will influence this vertex
  float m_Weights[MAX_BONE_INFLUENCE];   //> weights from each bone
};

} // end namespace AA

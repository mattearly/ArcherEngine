#pragma once
#include <glm/glm.hpp>
namespace AA {

struct UnlitVertex {
  UnlitVertex(glm::vec3 pos, glm::vec2 uv);
  glm::vec3 Position{};
  glm::vec2 TexCoords{};
};

struct LitVertex {
  LitVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal);
  glm::vec3 Position{};
  glm::vec2 TexCoords{};
  glm::vec3 Normal{};
};

struct TanVertex {
  TanVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal, glm::vec3 tan, glm::vec3 bitan);
  glm::vec3 Position{};
  glm::vec2 TexCoords{};
  glm::vec3 Normal{};
  glm::vec3 Tangent{};
  glm::vec3 BiTangent{};
};

constexpr auto MAX_BONE_INFLUENCE = 4;

struct AnimVertex : public LitVertex {
  AnimVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal);

  //bone indexes which will influence this vertex
  int m_BoneIDs[MAX_BONE_INFLUENCE];
  //weights from each bone
  float m_Weights[MAX_BONE_INFLUENCE];
};

} // end namespace AA

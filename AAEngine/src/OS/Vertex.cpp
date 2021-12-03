#include "Vertex.h"
#include <glm/glm.hpp>
using glm::vec3;
using glm::vec2;
namespace AA {

UnlitVertex::UnlitVertex(glm::vec3 pos, glm::vec2 uv) : Position(pos), TexCoords(uv) {}

LitVertex::LitVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal) : Position(pos), TexCoords(uv), Normal(normal) {}

TanVertex::TanVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal, glm::vec3 tan, glm::vec3 bitan) : Position(pos), TexCoords(uv), Normal(normal), Tangent(tan), BiTangent(bitan) {}

AnimVertex::AnimVertex(glm::vec3 pos, glm::vec2 uv, glm::vec3 normal) 
  : LitVertex(pos, uv, normal)
 {}

} // end namespace AA

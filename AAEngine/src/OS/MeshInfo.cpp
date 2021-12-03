#include "MeshInfo.h"
#include <glm\gtc\matrix_transform.hpp>
#include "../Utility/Conversions.h"
namespace AA {
MeshInfo::MeshInfo(unsigned int a, unsigned int elcount)
  : vao(a), numElements(elcount) {}

MeshInfo::MeshInfo(unsigned int a, unsigned int elcount, glm::mat4 trans)
  : vao(a), numElements(elcount), local_transform(trans) {}

MeshInfo::MeshInfo(unsigned int a, unsigned int elcount, std::unordered_map<unsigned int, std::string> t_id, glm::mat4 trans)
  : vao(a), numElements(elcount), textureDrawIds(t_id), local_transform(trans) {}

}  // end namespace AA

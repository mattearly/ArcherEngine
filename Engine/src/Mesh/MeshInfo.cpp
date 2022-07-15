#include "MeshInfo.h"
#include <glm\gtc\matrix_transform.hpp>
namespace AA {
MeshInfo::MeshInfo(unsigned int a, unsigned int elcount)
  : vao(a), numElements(elcount) {
}

MeshInfo::MeshInfo(unsigned int a, unsigned int elcount, glm::mat4 trans)
  : vao(a), numElements(elcount), local_transform(trans) {
}

MeshInfo::MeshInfo(unsigned int a, unsigned int elcount, TextureMapType t_id, glm::mat4 local_trans)
  : vao(a), numElements(elcount), textureDrawIds(t_id), local_transform(local_trans) {
}

MeshInfo::MeshInfo(unsigned int a, unsigned int elcount, TextureMapType t_id, Material mat, glm::mat4 local_trans)
  : vao(a), numElements(elcount), textureDrawIds(t_id), material(mat), local_transform(local_trans) {
}

}  // end namespace AA

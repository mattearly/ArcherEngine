#pragma once
#include "Material.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
namespace AA {
struct MeshInfo {
  MeshInfo() = delete;
  MeshInfo(unsigned int a, unsigned int elcount);
  MeshInfo(unsigned int a, unsigned int elcount, glm::mat4 trans);
  MeshInfo(unsigned int a, unsigned int elcount, std::unordered_map<unsigned int, std::string> t_id, glm::mat4 trans);
  MeshInfo(unsigned int a, unsigned int elcount, std::unordered_map<unsigned int, std::string> t_id, Material mat, glm::mat4 trans);
  unsigned int vao = 0;
  std::unordered_map<unsigned int, std::string> textureDrawIds{}; // list of texture ids(unsigned int) and type(string) in mem
  Material material{};
  unsigned int numElements = 0;
  float shininess = 1.0f;
  glm::mat4 local_transform = glm::mat4(1.0f);
};
}  // end namespace AA

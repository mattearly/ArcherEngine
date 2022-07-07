#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include "TextureTypes.h"
#include "Material.h"
namespace AA {
/// <summary>
/// Basic Struct of a Mesh within a Scene
/// </summary>
struct MeshInfo {
  MeshInfo() = delete;
  MeshInfo(unsigned int a, unsigned int elcount);
  MeshInfo(unsigned int a, unsigned int elcount, glm::mat4 trans);
  MeshInfo(unsigned int a, unsigned int elcount, TextureMapType t_id, glm::mat4 trans);
  MeshInfo(unsigned int a, unsigned int elcount, TextureMapType t_id, Material mat, glm::mat4 trans);
  unsigned int vao = 0;
  unsigned int numElements = 0;
  TextureMapType textureDrawIds{}; // map of <draw_id, details>
  Material material{};
  float shininess = 0.5f;
  glm::mat4 local_transform = glm::mat4(1.0f);
};
} // end namespace AA


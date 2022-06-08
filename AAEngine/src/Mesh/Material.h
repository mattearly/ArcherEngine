#pragma once
#include <glm/glm.hpp>
#include <vector>
namespace AA {
struct Material {
public:
  ~Material();
  glm::vec3 Ambient{};
  glm::vec3 Diffuse{};
  glm::vec3 Specular{};
  glm::vec3 Emission{};
  // float SpecularHightlights{};  // 0 to 1000   - aka shininess
  // float OpticalDensity{};       // 0.001 to 10 - aka refraction index
  // float Dissolve{};             // 1.0 to 0    - aka alpha color
  // int IlluminationModel{};      // ignore
  // texture maps to blend with  - data not saved, see TextureInfo.h
};
}


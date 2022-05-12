#pragma once
#include <glm/glm.hpp>
namespace AA {
class SunLight {
public:
  SunLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);
  ~SunLight();

  glm::vec3 Direction, Ambient, Diffuse, Specular;

  // directional shadows
  bool Shadows = true;

  // oglshadersettings
  const unsigned int& GetFBO() const;
  const unsigned int& GetTexID() const;
  unsigned int ShadowWidth;
  unsigned int ShadowHeight;
  float ShadowNearPlane, ShadowFarPlane;
  float ShadowOrthoSize;

};
} // end namespace AA
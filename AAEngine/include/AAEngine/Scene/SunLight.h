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
  void SetShadowBiasMin(float min) noexcept;
  void SetShadowBiasMax(float max) noexcept;

  unsigned int ShadowWidth, ShadowHeight;
  float ShadowNearPlane, ShadowFarPlane;
  float ShadowOrthoSize;

private:
  float ShadowBiasMin, ShadowBiasMax;

};
} // end namespace AA
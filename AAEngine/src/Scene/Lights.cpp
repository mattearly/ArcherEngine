#include "Lights.h"

namespace AA {

DirectionalLight::DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
  : Direction(dir), Ambient(amb), Diffuse(diff), Specular(spec) {}

SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, float inner,
  float outer, float constant, float linear, float quad,
  glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
  : Position(pos), Direction(dir), CutOff(inner), OuterCutOff(outer),
  Constant(constant), Linear(linear), Quadratic(quad), Ambient(amb),
  Diffuse(diff), Specular(spec) {
  static int uniqueSpotId = 0;
  id = uniqueSpotId;
  uniqueSpotId++;
}

PointLight::PointLight(glm::vec3 pos, float constant, float linear, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
  : Position(pos), Constant(constant), Linear(linear), Quadratic(quad), Ambient(amb), Diffuse(diff), Specular(spec) {
  static int uniquePointId = 0;
  id = uniquePointId;
  uniquePointId++;
}

}
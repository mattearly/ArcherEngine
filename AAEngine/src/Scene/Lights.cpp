#include "Lights.h"
#include "../OS/OpenGL/Graphics.h"

namespace AA {

unsigned int ShadowDepthMapFBO = 0;
unsigned int ShadowDepthMapTextureId = 0;

DirectionalLight::DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
  : Direction(dir), Ambient(amb), Diffuse(diff), Specular(spec) {

  Shadows = true;

  ShadowNearPlane = 1.f;
  ShadowFarPlane = 7.5f;
  ShadowOrthoSize = 10.f;

  ShadowWidth = 1024;
  ShadowHeight = 1024;

  if (ShadowDepthMapFBO != 0 && ShadowDepthMapTextureId != 0) {
    return;  // ALREADY SET
  }

  // init if needed
  ShadowDepthMapFBO = OpenGL::CreateDepthMapFBO(ShadowWidth, ShadowHeight, ShadowDepthMapTextureId);
}

DirectionalLight::~DirectionalLight() {
  if (ShadowDepthMapFBO != 0) {
    OpenGL::DeleteFramebuffer(1u, ShadowDepthMapFBO);
  }
  if (ShadowDepthMapTextureId != 0) {
    OpenGL::DeleteTex(1u, ShadowDepthMapTextureId);
    ShadowDepthMapTextureId = 0;
  }
}

const unsigned int& DirectionalLight::GetFBO() const {
  return ShadowDepthMapFBO;
}

const unsigned int& DirectionalLight::GetTexID() const {
  return ShadowDepthMapTextureId;
}

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
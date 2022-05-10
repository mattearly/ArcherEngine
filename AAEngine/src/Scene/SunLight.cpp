#include "SunLight.h"
#include "../OS/OpenGL/Graphics.h"
namespace AA {

unsigned int ShadowDepthMapFBO = 0;
unsigned int ShadowDepthMapTextureId = 0;

SunLight::SunLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
  : Direction(dir), Ambient(amb), Diffuse(diff), Specular(spec) {

  Shadows = true;

  ShadowNearPlane = 1.0f;
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

SunLight::~SunLight() {
  // remove from graphics card on destroy
  if (ShadowDepthMapFBO != 0) {  
    OpenGL::DeleteFramebuffer(1u, ShadowDepthMapFBO);
    ShadowDepthMapFBO = 0;
  }
  if (ShadowDepthMapTextureId != 0) {
    OpenGL::DeleteTex(1u, ShadowDepthMapTextureId);
    ShadowDepthMapTextureId = 0;
  }
}

const unsigned int& SunLight::GetFBO() const {
  return ShadowDepthMapFBO;
}

const unsigned int& SunLight::GetTexID() const {
  return ShadowDepthMapTextureId;
}
} // end namespace AA
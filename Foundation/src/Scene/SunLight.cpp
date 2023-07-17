#include <Scene/SunLight.h>
#include "../OS/OpenGL/Graphics.h"
#include "../OS/OpenGL/InternalShaders/Uber.h"
namespace AA {

unsigned int ShadowDepthMapFBO = 0;
unsigned int ShadowDepthMapTextureId = 0;

SunLight::SunLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
  : Direction(dir), Ambient(amb), Diffuse(diff), Specular(spec) {

  Shadows = true;

  ShadowNearPlane = 1.0f;
  ShadowFarPlane = 7.5f * 100;
  ShadowOrthoSize = 500.f;

  ShadowWidth = 1024;
  ShadowHeight = 1024;

  ShadowBiasMin = 0.025f;
  ShadowBiasMax = 0.0005f;

  auto uber_shader = InternalShaders::Uber::Get();
  uber_shader->SetInt("u_is_dir_light_on", 1);
  uber_shader->SetVec3("u_dir_light.Direction", Direction);
  uber_shader->SetVec3("u_dir_light.Ambient", Ambient);
  uber_shader->SetVec3("u_dir_light.Diffuse", Diffuse);
  uber_shader->SetVec3("u_dir_light.Specular", Specular);
  uber_shader->SetBool("u_dir_light.Shadows", Shadows);
  uber_shader->SetFloat("u_dir_light.ShadowBiasMin", ShadowBiasMin);
  uber_shader->SetFloat("u_dir_light.ShadowBiasMax", ShadowBiasMax);


  if (ShadowDepthMapFBO != 0 && ShadowDepthMapTextureId != 0) {
    return;  // ALREADY SET
  }

  // init if needed
  ShadowDepthMapFBO = OpenGL::GetGL()->CreateDepthMapFBO(ShadowWidth, ShadowHeight, ShadowDepthMapTextureId);
}

SunLight::~SunLight() {
  // remove from graphics card on destroy
  if (ShadowDepthMapFBO != 0) {
    OpenGL::GetGL()->DeleteFramebuffer(1u, ShadowDepthMapFBO);
    ShadowDepthMapFBO = 0;
  }
  if (ShadowDepthMapTextureId != 0) {
    OpenGL::GetGL()->DeleteTex(1u, ShadowDepthMapTextureId);
    ShadowDepthMapTextureId = 0;
  }
}

const unsigned int& SunLight::GetFBO() const {
  return ShadowDepthMapFBO;
}

const unsigned int& SunLight::GetTexID() const {
  return ShadowDepthMapTextureId;
}
void SunLight::SetShadowBiasMin(float min) noexcept {
  ShadowBiasMin = min;
  InternalShaders::Uber::Get()->SetFloat("u_dir_light.ShadowBiasMin", min);
}
void SunLight::SetShadowBiasMax(float max) noexcept {
  ShadowBiasMax = max;
  InternalShaders::Uber::Get()->SetFloat("u_dir_light.ShadowBiasMax", max);
}
} // end namespace AA
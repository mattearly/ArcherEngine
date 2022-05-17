#pragma once
#include <AAEngine/Interface.h>
#include <imgui.h>

static struct Sunlight {
  AA::Interface* interface_ref = nullptr;
  float light_direction[3] = { -.33f, -1.f, -.33f };
  float light_ambient[3] = { .003f, .003f, .003f };
  float light_diffuse[3] = { .3f, .3f, .3f };
  float light_specular[3] = { .5f, .5f, .5f };

  bool shadow_on = true;

  float shadow_far_plane = 1024.f;

  float shadow_ortho_size = 1024.f;

  float shadow_bias_min = .00025f;
  float shadow_bias_max_multi = .025f;

} * sun;


void load_sun(AA::Interface& interface) {
  if (!sun) { 
    sun = new Sunlight(); 
    sun->interface_ref = &interface;
  } else { return; }

  // a sunlight
  interface.SetSunLight(
    glm::vec3(sun->light_direction[0], sun->light_direction[1], sun->light_direction[2]),
    glm::vec3(sun->light_ambient[0], sun->light_ambient[1], sun->light_ambient[2]),
    glm::vec3(sun->light_diffuse[0], sun->light_diffuse[1], sun->light_diffuse[2]),
    glm::vec3(sun->light_specular[0], sun->light_specular[1], sun->light_specular[2])
  );

  // set shadow defaults
  {
    auto strong = sun->interface_ref->GetSunLight().lock();
    strong->Shadows = sun->shadow_on;
    strong->ShadowFarPlane = sun->shadow_far_plane;
    strong->ShadowOrthoSize = sun->shadow_ortho_size;
    strong->SetShadowBiasMin(sun->shadow_bias_min);
    strong->SetShadowBiasMax(sun->shadow_bias_max_multi);
  }

  sun->interface_ref->AddToImGuiUpdate([]() {
    ImGui::Begin("SUNLIGHT");
    bool updated_dir = ImGui::SliderFloat3("Direction", sun->light_direction, -1.f, 1.f, "%f", 1.f);
    bool updated_amb = ImGui::SliderFloat3("Ambient", sun->light_ambient, .003f, 1.f, "%f", 1.f);
    bool updated_diff = ImGui::SliderFloat3("Diffuse", sun->light_diffuse, .003f, 1.f, "%f", 1.f);
    bool updated_spec = ImGui::SliderFloat3("Specular", sun->light_specular, .003f, 1.f, "%f", 1.f);
    bool updated_shadows = ImGui::Checkbox("RenderSunShadows", &sun->shadow_on);
    bool updated_shadow_far = ImGui::SliderFloat("ShadowFarPlane", &sun->shadow_far_plane, 10.f, 750.f, "%f", 1.0f);
    bool updated_shadow_bias_min = ImGui::SliderFloat("ShadowBiasMin", &sun->shadow_bias_min, 0.00005f, 0.06f, "%f", 1.0f);
    bool updated_shadow_bias_max = ImGui::SliderFloat("ShadowBiasMax", &sun->shadow_bias_max_multi, 0.001f, 0.06f, "%f", 1.0f);
    bool updated_shadow_ortho_size = ImGui::SliderFloat("ShadowOrthoSize", &sun->shadow_ortho_size, 10.f, 2048.f, "%f", 1.0f);
    ImGui::End();

    // update as needed
    if (updated_dir || updated_amb || updated_diff || updated_spec) {
      sun->interface_ref->SetSunLight(
        glm::vec3(sun->light_direction[0], sun->light_direction[1], sun->light_direction[2]),
        glm::vec3(sun->light_ambient[0], sun->light_ambient[1], sun->light_ambient[2]),
        glm::vec3(sun->light_diffuse[0], sun->light_diffuse[1], sun->light_diffuse[2]),
        glm::vec3(sun->light_specular[0], sun->light_specular[1], sun->light_specular[2])
      );
    }
    if (updated_shadows) {
      auto strong_ref = sun->interface_ref->GetSunLight().lock();
      strong_ref->Shadows = sun->shadow_on;
    }
    if (updated_shadows) {
      auto strong = sun->interface_ref->GetSunLight().lock();
      strong->Shadows = sun->shadow_on;
    }
    if (updated_shadow_far) {
      auto strong = sun->interface_ref->GetSunLight().lock();
      strong->ShadowFarPlane = sun->shadow_far_plane;
    }
    if (updated_shadow_bias_min) {
      auto strong = sun->interface_ref->GetSunLight().lock();
      strong->SetShadowBiasMin(sun->shadow_bias_min);
    }
    if (updated_shadow_bias_max) {
      auto strong = sun->interface_ref->GetSunLight().lock();
      strong->SetShadowBiasMax(sun->shadow_bias_max_multi);
    }
    if (updated_shadow_ortho_size) {
      auto strong = sun->interface_ref->GetSunLight().lock();
      strong->ShadowOrthoSize = sun->shadow_ortho_size;
    }
    });


}

void unload_sun() {
  delete sun;
  sun = nullptr;
}

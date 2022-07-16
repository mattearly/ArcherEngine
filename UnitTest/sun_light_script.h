#pragma once
#include <ArcherEngine/Interface.h>
#include <imgui.h>

static struct Sunlight {
  AA::Interface* interface_ref = nullptr;
  float light_direction[3] = { -.33f, -1.0f, -.33f };
  float light_ambient[3] = { .099f, .099f, .099f };
  float light_diffuse[3] = { .43f, .43f, .43f };
  float light_specular[3] = { .5f, .5f, .5f };

  bool shadow_on = true;

  float shadow_far_plane = 1024.f;

  float shadow_ortho_size = 505.f;

  float shadow_bias_min = 0.00025f;
  float shadow_bias_max_multi = 0.001f;

} *sun;

void load_sun(AA::Interface& interface, bool daycyclehack = false) {
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

    static bool vec_lock = true;
    bool updated_update_veclock = ImGui::Checkbox("vec lock", &vec_lock);

    bool updated_amb = false;
    bool updated_diff = false;
    bool updated_spec = false;
    if (vec_lock) {
      updated_amb = ImGui::SliderFloat("Ambient", sun->light_ambient, .003f, 1.f, "%f", 1.f);
      updated_diff = ImGui::SliderFloat("Diffuse", sun->light_diffuse, .003f, 1.f, "%f", 1.f);
      updated_spec = ImGui::SliderFloat("Specular", sun->light_specular, .003f, 1.f, "%f", 1.f);
    } else {
      updated_amb = ImGui::SliderFloat3("Ambient", sun->light_ambient, .003f, 1.f, "%f", 1.f);
      updated_diff = ImGui::SliderFloat3("Diffuse", sun->light_diffuse, .003f, 1.f, "%f", 1.f);
      updated_spec = ImGui::SliderFloat3("Specular", sun->light_specular, .003f, 1.f, "%f", 1.f);
    }
    bool updated_shadows = ImGui::Checkbox("RenderSunShadows", &sun->shadow_on);
    bool updated_shadow_far = ImGui::SliderFloat("ShadowFarPlane", &sun->shadow_far_plane, 10.f, 2048.f, "%f", 1.0f);
    bool updated_shadow_bias_min = ImGui::SliderFloat("ShadowBiasMin", &sun->shadow_bias_min, 0.00005f, 0.06f, "%f", 1.0f);
    bool updated_shadow_bias_max = ImGui::SliderFloat("ShadowBiasMax", &sun->shadow_bias_max_multi, 0.001f, 0.06f, "%f", 1.0f);
    bool updated_shadow_ortho_size = ImGui::SliderFloat("ShadowOrthoSize", &sun->shadow_ortho_size, 10.f, 2048.f, "%f", 1.0f);
    ImGui::End();

    // update as needed
    if (updated_dir || updated_amb || updated_diff || updated_spec) {
      if (vec_lock) {
        sun->light_ambient[2] = sun->light_ambient[1] = sun->light_ambient[0];
        sun->light_diffuse[2] = sun->light_diffuse[1] = sun->light_diffuse[0];
        sun->light_specular[2] = sun->light_specular[1] = sun->light_specular[0];
      }

      sun->interface_ref->SetSunLight(
        glm::vec3(sun->light_direction[0], sun->light_direction[1], sun->light_direction[2]),
        glm::vec3(sun->light_ambient[0], sun->light_ambient[1], sun->light_ambient[2]),
        glm::vec3(sun->light_diffuse[0], sun->light_diffuse[1], sun->light_diffuse[2]),
        glm::vec3(sun->light_specular[0], sun->light_specular[1], sun->light_specular[2])
      );

    }
    if (updated_shadows) {
      sun->interface_ref->GetSunLight().lock()->Shadows = sun->shadow_on;
    }
    if (updated_shadow_far) {
      sun->interface_ref->GetSunLight().lock()->ShadowFarPlane = sun->shadow_far_plane;
    }
    if (updated_shadow_bias_min) {
      sun->interface_ref->GetSunLight().lock()->SetShadowBiasMin(sun->shadow_bias_min);
    }
    if (updated_shadow_bias_max) {
      sun->interface_ref->GetSunLight().lock()->SetShadowBiasMax(sun->shadow_bias_max_multi);
    }
    if (updated_shadow_ortho_size) {
      sun->interface_ref->GetSunLight().lock()->ShadowOrthoSize = sun->shadow_ortho_size;
    }
    });

  if (daycyclehack) {
    sun->interface_ref->AddToUpdate([](float dt) {
      static float passed_time = 0.f;
      passed_time += dt;

      sun->interface_ref->SetSunLight(
        glm::vec3(sin(passed_time), sun->light_direction[1], cos(passed_time)),
        glm::vec3(sun->light_ambient[0], sun->light_ambient[1], sun->light_ambient[2]),
        glm::vec3(sun->light_diffuse[0], sun->light_diffuse[1], sun->light_diffuse[2]),
        glm::vec3(sun->light_specular[0], sun->light_specular[1], sun->light_specular[2])
      );
      });
  }
}

void unload_sun() {
  delete sun;
  sun = nullptr;
}

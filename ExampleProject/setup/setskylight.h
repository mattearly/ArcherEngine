//#pragma once
//#include "../Manager.h"
//
//static bool is_skylight_setup = false;
//static bool is_skylight_on = false;
//static glm::vec3 dir_light_direction = glm::vec3(.35f, -.35f, .35f);
//static glm::vec3 dir_light_ambient = glm::vec3(0.04f);
//static glm::vec3 dir_light_diff = glm::vec3(0.37f);
//static glm::vec3 dir_light_spec = glm::vec3(0.6f);
//
//void setskylight() {
//  if (is_skylight_setup)
//    return;
//
//  {
//    // prepare a skybox cubemap
//    static const std::string skyboxfolder = "res/skybox/night/";
//    static const std::string order[6] = { "right", "left", "top", "bottom", "front", "back" };
//    static const std::string skyboxfileext = ".png";
//    static std::vector<std::string> cubemapfiles;
//    cubemapfiles.reserve(6);
//    for (int j = 0; j < 6; ++j) {
//      cubemapfiles.emplace_back(skyboxfolder + order[j] + skyboxfileext);
//    }
//
//    if (is_skylight_on) {
//      Engine.SetDirectionalLight(dir_light_direction, dir_light_ambient, dir_light_diff, dir_light_spec);
//      Engine.SetSkybox(cubemapfiles);
//    }
//
//    Engine.AddToImGuiUpdate([]() {
//      ImGui::Begin("Directional Sky Light");
//      bool toggle = ImGui::Checkbox("On", &is_skylight_on);
//      bool dirchanged = ImGui::SliderFloat3("direction", &dir_light_direction[0], -1.f, 1.f);
//      bool ambchanged = ImGui::SliderFloat3("ambient", &dir_light_ambient[0], 0.f, 1.f);
//      bool diffchanged = ImGui::SliderFloat3("diffuse", &dir_light_diff[0], 0.f, 1.f);
//      bool specchanged = ImGui::SliderFloat3("specular", &dir_light_spec[0], 0.f, 64.f);
//      ImGui::End();
//      if (toggle)
//      {
//        if (is_skylight_on) {
//          Engine.RemoveDirectionalLight();
//          Engine.RemoveSkybox();
//        } else {
//          Engine.SetDirectionalLight(dir_light_direction, dir_light_ambient, dir_light_diff, dir_light_spec);
//          Engine.SetSkybox(cubemapfiles);
//        }
//      }
//
//      if (specchanged || diffchanged || ambchanged || dirchanged && is_skylight_on) {
//        Engine.SetDirectionalLight(dir_light_direction, dir_light_ambient, dir_light_diff, dir_light_spec);
//      }
//
//    });
//  }
//
//  is_skylight_setup = true;
//}
//

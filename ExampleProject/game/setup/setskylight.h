#pragma once
#include <string>
#include <vector>
#include <memory>
#include <AncientArcher/AncientArcher.h>
extern AA::AncientArcher instance;
static bool is_skylight_setup = false;
static bool is_skylight_on = true;
static glm::vec3 dir_light_direction = glm::vec3(.55f, -.75f, 1.f);
static glm::vec3 dir_light_ambient = glm::vec3(.04f);
static glm::vec3 dir_light_diff = glm::vec3(.37f);
static glm::vec3 dir_light_spec = glm::vec3(0.1f);

void setskylight() {
  if (is_skylight_setup)
    return;

  {
    // prepare a skybox cubemap
    static const std::string skyboxfolder = "res/skybox/night/";
    static const std::string order[6] = { "right", "left", "top", "bottom", "front", "back" };
    static const std::string skyboxfileext = ".png";
    static std::vector<std::string> cubemapfiles;
    cubemapfiles.reserve(6);
    for (int j = 0; j < 6; ++j) {
      cubemapfiles.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    if (is_skylight_on) {
      instance.SetDirectionalLight(dir_light_direction, dir_light_ambient, dir_light_diff, dir_light_spec);
      instance.SetSkybox(cubemapfiles);
    }

    static float dir_light_toggle_timer = 0.f;
    instance.AddToKeyHandling([](AA::KeyboardButtons& kb) {
      const float cd = .7f;
      if (kb.f1 && dir_light_toggle_timer > cd) {
        if (is_skylight_on) {
          instance.RemoveDirectionalLight();
          instance.RemoveSkybox();
          is_skylight_on = false;
        } else {
          instance.SetDirectionalLight(dir_light_direction, dir_light_ambient, dir_light_diff, dir_light_spec);
          instance.SetSkybox(cubemapfiles);
          is_skylight_on = true;
        }
        dir_light_toggle_timer = 0.f; //reset timer if success
      }
    }
    );
    instance.AddToUpdate([](float dt) { dir_light_toggle_timer += dt; });
  }

  is_skylight_setup = true;
}


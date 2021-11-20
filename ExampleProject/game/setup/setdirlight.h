#pragma once
#include <AncientArcher/AncientArcher.h>
#ifdef _DEBUG
#include <iostream>
#endif

bool is_dir_light_setup = false;
bool dir_light_on = true;
glm::vec3 dir_light_direction = glm::vec3(.55f, -.75f, 1.f);
glm::vec3 dir_light_ambient = glm::vec3(.04f);
glm::vec3 dir_light_diff = glm::vec3(.37f);
glm::vec3 dir_light_spec = glm::vec3(0.1f);
extern AA::AncientArcher instance;

void setdirlight() {

  if (is_dir_light_setup)
    return;

  if (dir_light_on) instance.SetDirectionalLight(dir_light_direction, dir_light_ambient, dir_light_diff, dir_light_spec);

  static float dir_light_toggle_timer = 0.f;
  instance.AddToKeyHandling([](AA::KeyboardButtons& kb) {
    const float cd = .5f;
    if (kb.f1 && dir_light_toggle_timer > cd) {
      if (dir_light_on) {
        instance.RemoveDirectionalLight();
#ifdef _DEBUG
        std::cout << "dir light turned off\n";
#endif
        dir_light_on = false;
      } else {
        instance.SetDirectionalLight(dir_light_direction, dir_light_ambient, dir_light_diff, dir_light_spec);
#ifdef _DEBUG
        std::cout << "dir light turned on\n";
#endif
        dir_light_on = true;
      }
      dir_light_toggle_timer = 0.f; //reset timer if success
    }
  }
  );

  instance.AddToUpdate([](float dt) { dir_light_toggle_timer += dt; });

  is_dir_light_setup = true;
}


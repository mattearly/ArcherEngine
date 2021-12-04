#pragma once
#include <AncientArcher/AncientArcher.h>
#include "MainCharacter.h"
#include "setup/setskylight.h"
#include "setup/setmodels.h"
#include "setup/setfullscreentoggle.h"

MainCharacter my_character;

struct MainGame {

  void Setup() {
    
    // helper show keybindings
    instance.AddToImGuiUpdate([](){ 
      ImGui::Begin("Keybindings");
      ImGui::Text("wasd - walk");
      ImGui::Text("tab - inventory/mouse control");
      ImGui::Text("shift - run");
      ImGui::Text("f - toggle flashlight");
      ImGui::Text("f1 - toggle sunlight");
      ImGui::Text("f11 - toggle fullscreen");
      ImGui::End();
    });


    my_character.Setup();

    setmodels();
    
    setskylight();
    
    setfullscreentoggle();
  };

  void Update(float dt) {
    my_character.Update(dt);
  };

  void Teardown() {
    my_character.Teardown();
  };

};
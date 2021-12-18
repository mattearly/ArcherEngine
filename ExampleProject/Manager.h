#pragma once
#include <AncientArcher/AncientArcher.h>
#include <imgui.h>
#include "scenes/Intro.h"

extern AA::AncientArcher Engine;

struct Manager {

  Manager() {
    // helper show keybindings
    //Engine.AddToImGuiUpdate([]() {
    //  ImGui::Begin("Keybindings");
    //  ImGui::Text("wasd - walk");
    //  ImGui::Text("tab - inventory/mouse control");
    //  ImGui::Text("shift - run");
    //  ImGui::Text("f - toggle flashlight");
    //  ImGui::Text("f1 - toggle sunlight");
    //  ImGui::Text("f11 - toggle fullscreen");
    //  ImGui::End();
    //});

    //my_character.Setup();

    //setmodels();

    //setskylight();

    //setfullscreentoggle();

    //setmusic();
    state = STATE::PLAY_SCENE;
    scene_switch(SCENE::INTRO);
  }

  enum class STATE { OFF, PLAY_SCENE } state;

  enum class SCENE { INTRO, MAIN_MENU, CHARACTER, LEVEL1 } scene;

  void tick(const float& dt) {
    if (state == STATE::PLAY_SCENE) {
      switch (scene) {
      case SCENE::INTRO:
        TickIntro(dt);
        break;
      case SCENE::MAIN_MENU:
        //PlayMenu();
        break;
      case SCENE::CHARACTER:
        //PlayCharacter();
        break;
      case SCENE::LEVEL1:
        //PlayLevel1();
        break;
      }
    } else {
      Engine.Shutdown();
    }
  }


  void scene_switch(const SCENE& to) {
    static bool first_time = true;
    if (scene == to && !first_time) {
      return; // already on this scene
    }

    // LOAD NEW SCENE
    switch (to) {
    case SCENE::INTRO:
      LoadIntro();
      break;
    case SCENE::MAIN_MENU:
      //PlayMenu();
      break;
    case SCENE::CHARACTER:
      //PlayCharacter();
      break;
    case SCENE::LEVEL1:
      //PlayLevel1();
      break;
    }

    if (!first_time) {
      // UNLOAD OLD SCENE
      switch (scene) {
      case SCENE::INTRO:
        break;
      case SCENE::MAIN_MENU:
        //PlayMenu();
        break;
      case SCENE::CHARACTER:
        //PlayCharacter();
        break;
      case SCENE::LEVEL1:
        //PlayLevel1();
        break;
      }
    }

    if (first_time) {
      first_time = false;
    }

    scene = to;
  }

};
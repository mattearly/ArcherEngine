#pragma once
#include <AncientArcher/AncientArcher.h>
#include <imgui.h>
#include "scenes/Intro.h"
#include "scenes/Menu.h"

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
    static float timer = 0;
    timer += dt;
    if (state == STATE::PLAY_SCENE) {
      switch (scene) {
      case SCENE::INTRO:
        if (timer <= 3.f) {
          TickIntro(dt);
        } else {
          scene_switch(SCENE::MAIN_MENU);
        }
        break;
      case SCENE::MAIN_MENU:
        TickMenu(dt);
        break;
      case SCENE::CHARACTER:
        break;
      case SCENE::LEVEL1:
        break;
      }
    } else {
      Engine.Shutdown();
    }
  }


  void scene_switch(const SCENE& to) {
    static bool first_time = true;  // default true

    // unload the current (prev) scene
    if (!first_time) {
      switch (scene) {
      case SCENE::INTRO:
        UnloadIntro();
        break;
      case SCENE::MAIN_MENU:
        UnloadMenu();
        break;
      case SCENE::CHARACTER:
        break;
      case SCENE::LEVEL1:
        break;
      }
    }


    // load the new scene
    switch (to) {
    case SCENE::INTRO:
      LoadIntro();
      break;
    case SCENE::MAIN_MENU:
      LoadMenu();
      break;
    case SCENE::CHARACTER:
      break;
    case SCENE::LEVEL1:
      break;
    }

    first_time = false;
    scene = to;
  }

};
#pragma once
#include <AAEngine/Interface.h>
#include <AAEngine/WindowOptions.h>
#include <AAEngine/Utility/rand.h>
#include <AAEngine/Utility/SceneCheck.h>

#include <imgui.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <memory>
#include <algorithm>

#include "GameStates.h"
#include "scenes/Intro.h"
#include "scenes/Menu.h"
#include "scenes/Character.h"
#include "scenes/Level1.h"

extern AA::Interface g_engine;
extern SCENE g_next_scene;

struct Manager {
private:
  STATE current_state;
  SCENE current_scene;

public:
  Manager() {
    current_state = STATE::PLAY_SCENE;
    scene_switch(SCENE::INTRO);
  }

  void tick(const float& dt) {
    static float timer = 0;
    timer += dt;
    if (current_state == STATE::PLAY_SCENE) {

      if (current_scene != g_next_scene) {
        scene_switch(g_next_scene);
      }

      switch (current_scene) {
      case SCENE::INTRO:
        TickIntro(dt);
        break;
      case SCENE::MAIN_MENU:
        TickMenu(dt);
        break;
      case SCENE::CHARACTER:
        TickCharacter(dt);
        break;
      case SCENE::LEVEL1:
        TickLevel1(dt);
        break;
      }
    } else {
      g_engine.Shutdown();
    }
  }

  void scene_switch(const SCENE& to) {

    static bool first_time = true;  // default true

    // unload the current (prev) current_scene if one is loaded
    if (!first_time) {
      switch (current_scene) {
      case SCENE::INTRO:
        UnloadIntro();
        break;
      case SCENE::MAIN_MENU:
        UnloadMenu();
        break;
      case SCENE::CHARACTER:
        UnloadCharacter();
        break;
      case SCENE::LEVEL1:
        UnloadLevel1();
        break;
      }
    }

    // load the new current_scene
    switch (to) {
    case SCENE::INTRO:
      LoadIntro();
      break;
    case SCENE::MAIN_MENU:
      LoadMenu();
      break;
    case SCENE::CHARACTER:
      LoadCharacter();
      break;
    case SCENE::LEVEL1:
      LoadLevel1();
      break;
    }

    first_time = false;
    current_scene = to;
  }
};

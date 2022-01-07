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

#include "scenes/Intro.h"
#include "scenes/Menu.h"
#include "scenes/Character.h"
#include "scenes/Level1.h"

extern AA::Interface Engine;

struct Manager {
  Manager() {
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
        if (timer <= 8.f) {
          TickIntro(dt);
        } else {
          scene_switch(SCENE::MAIN_MENU);
        }
        break;
      case SCENE::MAIN_MENU:
        if (timer <= 10.f) {
          TickMenu(dt);
        } else {
          scene_switch(SCENE::CHARACTER);
        }        break;
      case SCENE::CHARACTER:
        if (timer <= 15.f) {
          TickCharacter(dt);
        } else {
          scene_switch(SCENE::LEVEL1);
        }
        break;
      case SCENE::LEVEL1:
        if (timer <= 20.f) {
          TickLevel1(dt);
        } else {
          Engine.Shutdown();
        }
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
        UnloadCharacter();
        break;
      case SCENE::LEVEL1:
        UnloadLevel1();
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
      LoadCharacter();
      break;
    case SCENE::LEVEL1:
      LoadLevel1();
      break;
    }

    first_time = false;
    scene = to;
  }
};
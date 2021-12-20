#pragma once
#include <AncientArcher/AncientArcher.h>
extern AA::AncientArcher Engine;

// local to this file
static bool menu_loaded = false;
static const char* menu_music_path = "res/acoustic_menu_dec_19_2021.ogg";
static const char* menu_logo_path = "res/neon_sphere.glb";
static int menu_logo_id = -1;

void LoadMenu() {
  Engine.AddMusic(menu_music_path);
  auto camid = Engine.AddCamera();
  Engine.SetCamPosition(camid, glm::vec3(0,0,40));
  menu_logo_id = Engine.AddProp(menu_logo_path);
  menu_loaded = true;
  Engine.PlayMusic();
}

void TickMenu(const float& dt) {
  if (menu_logo_id != -1) {
    static float curr_rot = 0.f;
    curr_rot += dt * 1.5;
    Engine.RotateProp(menu_logo_id, glm::vec3(0, curr_rot, 0));
  }
}

void UnloadMenu() {
  if (!menu_loaded) {
    return;
  }
  Engine.RemoveMusic();
  menu_loaded = false;
}
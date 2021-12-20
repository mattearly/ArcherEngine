#pragma once
#include <AncientArcher/AncientArcher.h>
extern AA::AncientArcher Engine;

// local to this file
static bool menu_loaded = false;
static const char* menu_music_path = "res/acoustic_menu_dec_19_2021.ogg";
static const char* menu_logo_path = "res/neon_sphere.glb";
static int menu_logo_id = -1;
static int menu_cam_id = -1;

void LoadMenu() {
  Engine.AddMusic(menu_music_path);
  menu_cam_id = Engine.AddCamera(Engine.GetWindowWidth(), Engine.GetWindowHeight());
  Engine.SetCamPosition(menu_cam_id, glm::vec3(0, 0, 40));
  menu_logo_id = Engine.AddProp(menu_logo_path);
  menu_loaded = true;
  Engine.PlayMusic();
}

void TickMenu(const float& dt) {
  if (menu_logo_id != -1) {
    static float curr_scale = 1.f;
    static float grow = true;
    if (grow) {
      curr_scale += dt;
      if (curr_scale > 10.f)
        grow = false;
    } else {
      curr_scale -= dt;
      if (curr_scale < 1.f)
        grow = true;
    }
    Engine.ScaleProp(menu_logo_id, glm::vec3(curr_scale));
  }
}

void UnloadMenu() {
  if (!menu_loaded) {
    return;
  }
  Engine.StopMusic();
  Engine.RemoveMusic();
  Engine.RemoveCamera(menu_cam_id);
  menu_cam_id = -1;
  Engine.RemoveProp(menu_logo_id);
  menu_logo_id = -1;
  menu_loaded = false;
}
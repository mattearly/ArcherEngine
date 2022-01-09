#pragma once
#include "../Manager.h"
extern AA::Interface g_engine;
extern SCENE g_next_scene;

// local to this file
static bool menu_loaded = false;
static const char* menu_music_path = "res/acoustic_menu_dec_19_2021.ogg";
static const char* menu_logo_path = "res/neon_sphere.glb";
static int menu_logo_id = -1;
static int menu_cam_id = -1;
static int gui_id = -1;

void LoadMenu() {
  gui_id = g_engine.AddToImGuiUpdate([]() {
    ImGui::Begin("Welcome!");
    bool new_game = ImGui::Button("New Game");
    bool continue_game = ImGui::Button("Continue");
    if (new_game) {
      g_next_scene = SCENE::CHARACTER;
    } else if (continue_game) {
      g_next_scene = SCENE::LEVEL1;
    }
    ImGui::End();
  });

  g_engine.AddMusic(menu_music_path);
  menu_cam_id = g_engine.AddCamera(g_engine.GetWindowWidth(), g_engine.GetWindowHeight());
  g_engine.SetCamPosition(menu_cam_id, glm::vec3(0, 0, 40));
  menu_logo_id = g_engine.AddProp(menu_logo_path);
  menu_loaded = true;
  g_engine.PlayMusic();
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
    g_engine.ScaleProp(menu_logo_id, glm::vec3(curr_scale));
  }
}

void UnloadMenu() {
  if (!menu_loaded) {
    return;
  }
  g_engine.StopMusic();
  g_engine.RemoveMusic();
  g_engine.RemoveCamera(menu_cam_id);
  menu_cam_id = -1;
  g_engine.RemoveProp(menu_logo_id);
  menu_logo_id = -1;
  g_engine.RemoveFromImGuiUpdate(gui_id);
  gui_id = -1;
  menu_loaded = false;

}
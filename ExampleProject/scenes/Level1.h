#pragma once
#include "../Manager.h"
extern AA::Interface Engine;

// local to this file
static bool level1_loaded = false;
static const char* level1_music_path = "res/acoustic_level1_jan_06_2022.ogg";
static const char* level1_logo_path = "res/peasant_man.dae";
static int level1_logo_id = -1;
static int level1_cam_id = -1;

void LoadLevel1() {
  Engine.AddMusic(level1_music_path);
  level1_cam_id = Engine.AddCamera(Engine.GetWindowWidth(), Engine.GetWindowHeight());
  Engine.SetCamPosition(level1_cam_id, glm::vec3(0, 0, 40));
  level1_logo_id = Engine.AddProp(level1_logo_path);
  level1_loaded = true;
  Engine.PlayMusic();
}

void TickLevel1(const float& dt) {
  if (level1_logo_id != -1) {
    static float curr_rot = 0.f;
    curr_rot += dt * 1.5f;
    Engine.RotateProp(level1_logo_id, glm::vec3(0, curr_rot, 0));
  }
}

void UnloadLevel1() {
  if (!level1_loaded) {
    return;
  }
  Engine.StopMusic();
  Engine.RemoveMusic();
  Engine.RemoveProp(level1_logo_id);
  level1_logo_id = -1;
  Engine.RemoveCamera(level1_cam_id);
  level1_cam_id = -1;
  level1_loaded = false;
}
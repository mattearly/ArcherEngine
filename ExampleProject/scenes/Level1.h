#pragma once
#include "../Manager.h"
extern AA::Interface g_engine;

// local to this file
static bool level1_loaded = false;
static const char* level1_music_path = "res/acoustic_level1_jan_06_2022.ogg";
static const char* level1_logo_path = "res/sphere.glb";
static int level1_logo_id = -1;
static int level1_cam_id = -1;
static int level1_spotlight_id = -1;

void LoadLevel1() {
  g_engine.SetWindowClearColor(glm::vec3(.1f, .1f, .67f));
  g_engine.AddMusic(level1_music_path);
  level1_cam_id = g_engine.AddCamera(g_engine.GetWindowWidth(), g_engine.GetWindowHeight());
  g_engine.SetCamPosition(level1_cam_id, glm::vec3(0, 0, 100));
  level1_logo_id = g_engine.AddProp(level1_logo_path);
  level1_spotlight_id = g_engine.AddSpotLight(glm::vec3(0), glm::vec3(0, -1, 0), 3.0f, 8.0f, 1.f, 0.045f, 0.0075f, glm::vec3(0.1f), glm::vec3(0.6f), glm::vec3(1));
  level1_loaded = true;
  g_engine.PlayMusic();
}

void TickLevel1(const float& dt) {
  if (level1_logo_id != -1) {
    static float curr_scale = 1.f;
    static float grow = true;
    if (grow) {
      curr_scale += dt;
      if (curr_scale > 3.f)
        grow = false;
    } else {
      curr_scale -= dt;
      if (curr_scale < 1.f)
        grow = true;
    }
    g_engine.ScaleProp(level1_logo_id, glm::vec3(curr_scale));
  }
}

void UnloadLevel1() {
  if (!level1_loaded) {
    return;
  }
  g_engine.StopMusic();
  g_engine.RemoveMusic();
  g_engine.RemoveProp(level1_logo_id);
  level1_logo_id = -1;
  g_engine.RemoveCamera(level1_cam_id);
  level1_cam_id = -1;
  g_engine.RemoveSpotLight(level1_spotlight_id);
  level1_spotlight_id = -1;
  level1_loaded = false;
}
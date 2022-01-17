#pragma once
#include "../Manager.h"
#include "../GameStates.h"
extern AA::Interface g_engine;
extern SCENE g_next_scene;

// local to this file
static bool intro_loaded = false;
static const char* intro_music_path = "res/acoustic_intro_dec_18_2021.ogg";
static const char* intro_logo_path = "res/neon_cube.glb";
static int intro_logo_id = -1;
static int intro_cam_id = -1;
static float intro_timer = 0.f;

void LoadIntro() {
  intro_timer = 0.f;
  g_engine.AddMusic(intro_music_path);
  intro_cam_id = g_engine.AddCamera(g_engine.GetWindowWidth(), g_engine.GetWindowHeight());
  g_engine.SetCamPosition(intro_cam_id, glm::vec3(0, 0, 40));
  intro_logo_id = g_engine.AddProp(intro_logo_path);
  g_engine.StencilProp(intro_logo_id, true);
  g_engine.StencilPropColor(intro_logo_id, glm::vec3(1.f, 0.f, 0.f));
  //g_engine.StencilPropWithNormals(intro_logo_id, false);  // defaults to false, no need to set
  g_engine.StencilPropScale(intro_logo_id, 1.05f);
  intro_loaded = true;
  g_engine.PlayMusic();
}

void TickIntro(const float& dt) {
  if (intro_logo_id != -1) {
    static float curr_rot = 0.f;
    curr_rot += dt * 1.1f;
    g_engine.RotateProp(intro_logo_id, glm::vec3(curr_rot, curr_rot, 0));
    g_engine.ScaleProp(intro_logo_id, glm::vec3(curr_rot));
  }

  intro_timer += dt;
  if (intro_timer > 7.89f) {
    g_next_scene = SCENE::MAIN_MENU;
  }
}

void UnloadIntro() {
  if (!intro_loaded) {
    return;
  }
  g_engine.StopMusic();
  g_engine.RemoveMusic();
  g_engine.RemoveProp(intro_logo_id);
  intro_logo_id = -1;
  g_engine.RemoveCamera(intro_cam_id);
  intro_cam_id = -1;
  intro_loaded = false;
}
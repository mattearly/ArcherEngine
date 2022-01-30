#pragma once
#include "../Manager.h"
extern AA::Interface g_engine;

// local to this file
static bool character_loaded = false;
static const char* character_music_path = "res/acoustic_character_jan_06_2022.ogg";
static const char* character_logo_path = "res/peasant_man.dae";
static int character_model_id = -1;
static int character_cam_id = -1;
static int character_spotlight_id = -1;

void LoadCharacter() {
  g_engine.SetWindowClearColor();
  g_engine.AddMusic(character_music_path);
  character_cam_id = g_engine.AddCamera(g_engine.GetWindowWidth(), g_engine.GetWindowHeight());
  auto cam = g_engine.GetCamera(character_cam_id);
  cam->SetPosition(glm::vec3(0, 0, 30));
  character_model_id = g_engine.AddProp(character_logo_path);
  g_engine.StencilProp(character_model_id, true);
  g_engine.StencilPropWithNormals(character_model_id, true);
  g_engine.StencilPropScale(character_model_id, 1.35f);
  g_engine.MoveProp(character_model_id, glm::vec3(0, -11, 0));
  g_engine.ScaleProp(character_model_id, glm::vec3(.1f));
  character_spotlight_id = g_engine.AddSpotLight(glm::vec3(0), glm::vec3(0, -1, 0), 3.0f, 8.0f, 1.f, 0.045f, 0.0075f, glm::vec3(0.1f), glm::vec3(0.6f), glm::vec3(1));
  character_loaded = true;
  g_engine.PlayMusic();
}

void TickCharacter(const float& dt) {
  if (character_model_id != -1) {
    static float curr_rot = 0.f;
    curr_rot += dt * 1.5f;
    g_engine.RotateProp(character_model_id, glm::vec3(0, curr_rot, 0));
  }
}

void UnloadCharacter() {
  if (!character_loaded) {
    return;
  }
  g_engine.StopMusic();
  g_engine.RemoveMusic();
  g_engine.RemoveProp(character_model_id);
  character_model_id = -1;
  g_engine.RemoveCamera(character_cam_id);
  character_cam_id = -1;
  g_engine.RemoveSpotLight(character_spotlight_id);
  character_spotlight_id = -1;
  character_loaded = false;
}

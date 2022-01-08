#pragma once
#include "../Manager.h"
extern AA::Interface Engine;

// local to this file
static bool character_loaded = false;
static const char* character_music_path = "res/acoustic_character_jan_06_2022.ogg";
static const char* character_logo_path = "res/peasant_man.dae";
static int character_model_id = -1;
static int character_cam_id = -1;
static int character_spotlight_id = -1;

void LoadCharacter() {
  Engine.SetWindowClearColor();
  Engine.AddMusic(character_music_path);
  character_cam_id = Engine.AddCamera(Engine.GetWindowWidth(), Engine.GetWindowHeight());
  Engine.SetCamPosition(character_cam_id, glm::vec3(0, 0, 30));
  character_model_id = Engine.AddProp(character_logo_path);
  Engine.MoveProp(character_model_id, glm::vec3(0, -11, 0));
  Engine.ScaleProp(character_model_id, glm::vec3(.1f));
  character_spotlight_id = Engine.AddSpotLight(glm::vec3(0), glm::vec3(0, -1, 0), 3.0f, 8.0f, 1.f, 0.045f, 0.0075f, glm::vec3(0.1f), glm::vec3(0.6f), glm::vec3(1));
  character_loaded = true;
  Engine.PlayMusic();
}

void TickCharacter(const float& dt) {
  if (character_model_id != -1) {
    static float curr_rot = 0.f;
    curr_rot += dt * 1.5f;
    Engine.RotateProp(character_model_id, glm::vec3(0, curr_rot, 0));
  }
}

void UnloadCharacter() {
  if (!character_loaded) {
    return;
  }
  Engine.StopMusic();
  Engine.RemoveMusic();
  Engine.RemoveProp(character_model_id);
  character_model_id = -1;
  Engine.RemoveCamera(character_cam_id);
  character_cam_id = -1;
  Engine.RemoveSpotLight(character_spotlight_id);
  character_spotlight_id = -1;
  character_loaded = false;
}
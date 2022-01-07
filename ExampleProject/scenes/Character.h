#pragma once
#include "../Manager.h"
extern AA::Interface Engine;

// local to this file
static bool character_loaded = false;
static const char* character_music_path = "res/acoustic_character_jan_06_2022.ogg";
static const char* character_logo_path = "res/sphere.glb";
static int character_logo_id = -1;
static int character_cam_id = -1;

void LoadCharacter() {
  Engine.AddMusic(character_music_path);
  character_cam_id = Engine.AddCamera(Engine.GetWindowWidth(), Engine.GetWindowHeight());
  Engine.SetCamPosition(character_cam_id, glm::vec3(0, 0, 40));
  character_logo_id = Engine.AddProp(character_logo_path);
  character_loaded = true;
  Engine.PlayMusic();
}

void TickCharacter(const float& dt) {
  if (character_logo_id != -1) {
    static float curr_rot = 0.f;
    curr_rot += dt * 1.5f;
    Engine.RotateProp(character_logo_id, glm::vec3(0, curr_rot, 0));
  }
}

void UnloadCharacter() {
  if (!character_loaded) {
    return;
  }
  Engine.StopMusic();
  Engine.RemoveMusic();
  Engine.RemoveProp(character_logo_id);
  character_logo_id = -1;
  Engine.RemoveCamera(character_cam_id);
  character_cam_id = -1;
  character_loaded = false;
}
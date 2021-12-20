#pragma once
#include <AncientArcher/AncientArcher.h>
extern AA::AncientArcher Engine;

// local to this file
static bool intro_loaded = false;
static const char* intro_music_path = "res/acoustic_intro_dec_18_2021.ogg";
static const char* intro_logo_path = "res/neon_cube.glb";
static int intro_logo_id = -1;
static int intro_cam_id = -1;

void LoadIntro() {
  Engine.AddMusic(intro_music_path);
  intro_cam_id = Engine.AddCamera(Engine.GetWindowWidth(), Engine.GetWindowHeight());
  Engine.SetCamPosition(intro_cam_id, glm::vec3(0,0,40));
  intro_logo_id = Engine.AddProp(intro_logo_path);
  intro_loaded = true;
  Engine.PlayMusic();
}

void TickIntro(const float& dt) {
  if (intro_logo_id != -1) {
    static float curr_rot = 0.f;
    curr_rot += dt * 1.5f;
    Engine.RotateProp(intro_logo_id, glm::vec3(0, curr_rot, 0));
  }
}

void UnloadIntro() {
  if (!intro_loaded) {
    return;
  }
  Engine.StopMusic();
  Engine.RemoveMusic();
  Engine.RemoveProp(intro_logo_id);
  intro_logo_id = -1;
  Engine.RemoveCamera(intro_cam_id);
  intro_cam_id = -1;
  intro_loaded = false;
}
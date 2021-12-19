#pragma once
#include <AncientArcher/AncientArcher.h>
extern AA::AncientArcher Engine;
static bool loaded = false;
const char* intro_music_path = "res/acoustic_intro_dec_18_2021.ogg";
const char* logo_path = "res/neon_cube.glb";
int logo_id = -1;

void LoadIntro() {
  Engine.AddMusic(intro_music_path);
  auto camid = Engine.AddCamera();
  Engine.SetCamPosition(camid, glm::vec3(0,0,40));
  logo_id = Engine.AddProp(logo_path);
  loaded = true;
  Engine.PlayMusic();
}

void TickIntro(const float& dt) {
  if (logo_id != -1) {
    static float curr_rot = 0.f;
    curr_rot += dt * 1.5;
    Engine.RotateProp(logo_id, glm::vec3(0, curr_rot, 0));
  }
}

void UnloadIntro() {
  if (!loaded) {
    return;
  }


  Engine.SoftReset();

  loaded = false;
}
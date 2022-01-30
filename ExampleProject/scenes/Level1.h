#pragma once
#include "../Manager.h"
extern AA::Interface g_engine;

// local to this file
static bool level1_loaded = false;
static const char* level1_music_path = "res/acoustic_level1_jan_06_2022.ogg";
static const char* level1_logo_path = "res/Zombie Punching.fbx";
static int level1_zombie_model_id = -1;
static const char* level1_sphere_model_path = "res/neon_sphere.glb";
static int level1_sphere_model_id = -1;
static int punching_anim_id = -1;
static int level1_cam_id = -1;
static int level1_reverse_cam_id = -1;
static int level1_spotlight_id = -1;

void LoadLevel1() {
  g_engine.SetWindowClearColor(glm::vec3(.1f, .1f, .67f));
  g_engine.AddMusic(level1_music_path);
  level1_cam_id = g_engine.AddCamera(g_engine.GetWindowWidth(), g_engine.GetWindowHeight());
  auto cam = g_engine.GetCamera(level1_cam_id);
  cam->SetPosition(glm::vec3(0, 0, 100));
  cam->SetKeepCameraToWindowSize(true);

  level1_reverse_cam_id = g_engine.AddCamera(300,200);
  auto cam2 = g_engine.GetCamera(level1_reverse_cam_id);
  cam2->SetPosition(glm::vec3(0, 0, 100));
  cam2->ShiftPitchAndYaw(0.f, 180.f);

  // set to middle of screen viewport
  cam2->SetBottomLeft((int)((g_engine.GetWindowWidth() / 2.f) - 150), (int)((g_engine.GetWindowHeight() / 2.f) - 100));

  g_engine.AddToKeyHandling([](AA::KeyboardButtons& kb) {
    if (kb.spacebar) {
      auto cam2_live = g_engine.GetCamera(level1_reverse_cam_id);
      cam2_live->SetBottomLeft((int)((g_engine.GetWindowWidth() / 2.f) - 150), (int)((g_engine.GetWindowHeight() / 2.f) - 100));
    }
    });


  // first model
  level1_zombie_model_id = g_engine.AddAnimProp(level1_logo_path);
  punching_anim_id = g_engine.AddAnimation(level1_logo_path, level1_zombie_model_id);
  g_engine.SetAnimationOnAnimProp(punching_anim_id, level1_zombie_model_id);
  g_engine.MoveAnimProp(level1_zombie_model_id, glm::vec3(0, -20, -5));
  g_engine.StencilAnimProp(level1_zombie_model_id, true);
  g_engine.StencilAnimPropWithNormals(level1_zombie_model_id, true);
  g_engine.StencilAnimPropScale(level1_zombie_model_id, 1.45f);
  g_engine.ScaleAnimProp(level1_zombie_model_id, glm::vec3(.3f));

  // second model
  level1_sphere_model_id = g_engine.AddProp(level1_sphere_model_path);
  g_engine.MoveProp(level1_sphere_model_id, glm::vec3(0, 0,160));
  //todo: fix texture issue


  level1_spotlight_id = g_engine.AddSpotLight(glm::vec3(0), glm::vec3(0, -1, 0), 3.0f, 8.0f, 1.f, 0.045f, 0.0075f, glm::vec3(0.1f), glm::vec3(0.6f), glm::vec3(1));
  level1_loaded = true;
  g_engine.PlayMusic();
}

void TickLevel1(const float& dt) {

  if (level1_sphere_model_id != -1) {
    static float curr_scale = 1.f;
    static float grow = true;
    if (grow) {
      curr_scale += dt*2;
      if (curr_scale > 30.f)
        grow = false;
    }
    else {
      curr_scale -= dt;
      if (curr_scale < 1.f)
        grow = true;
    }
    g_engine.ScaleProp(level1_sphere_model_id, glm::vec3(curr_scale));
  }






  //if (level1_logo_id != -1) {
  //  static float curr_scale = 1.f;
  //  static float grow = true;
  //  if (grow) {
  //    curr_scale += dt;
  //    if (curr_scale > 3.f)
  //      grow = false;
  //  } else {
  //    curr_scale -= dt;
  //    if (curr_scale < 1.f)
  //      grow = true;
  //  }
  //  g_engine.ScaleAnimProp(level1_logo_id, glm::vec3(curr_scale));
  //}
}

void UnloadLevel1() {
  if (!level1_loaded) {
    return;
  }
  g_engine.StopMusic();
  g_engine.RemoveMusic();
  g_engine.RemoveProp(level1_zombie_model_id);
  level1_zombie_model_id = -1;
  g_engine.RemoveCamera(level1_cam_id);
  level1_cam_id = -1;
  g_engine.RemoveCamera(level1_reverse_cam_id);
  level1_reverse_cam_id = -1;
  g_engine.RemoveSpotLight(level1_spotlight_id);
  level1_spotlight_id = -1;
  level1_loaded = false;
}
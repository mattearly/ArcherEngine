#pragma once
#include <AncientArcher/AncientArcher.h>
#include <AncientArcher/Utility/SceneCheck.h>
#include <glm/glm.hpp>
#include <imgui.h>
extern AA::AncientArcher instance;

bool is_models_set = false;
void setmodels() {
  if (is_models_set)
    return;
  is_models_set = true;

  /// <summary>
  /// Places static ground, re-using a plane 
  /// </summary>
  {
    const int half_repeat = 4;  // the number of tiles generated around the point 0,0 is double this number
    const int plane_stride = 400;
    for (int i = -plane_stride * half_repeat; i < plane_stride * half_repeat * 2; i += plane_stride) {
      for (int j = -plane_stride * half_repeat; j < plane_stride * half_repeat * 2; j += plane_stride) {
        int large_green_plane = instance.AddProp("res/large_green_plane.obj", glm::vec3(i, -99, j));
      }
    }
  }


  /// <summary>
  /// Places a wall of neon cubes with physics
  /// </summary>
  {
    const int scale = 10;
    const int tall = 10;
    const int length = 10;
    for (int i = 0; i < tall; i++) {
      for (int j = 0; j < length; j++) {
        auto tmp_id = instance.AddProp("res/neon_cube.glb", glm::vec3(i*scale*2.f, -99 + j*scale*2.f, 0), glm::vec3(scale*0.75f));
        instance.AddPropPhysics(tmp_id);
      }
    }
    instance.SimulateWorldPhysics(true);
  }
  

  /// <summary>
  /// Animated Model Test
  /// </summary>
  {
    static glm::vec3 base_loc = glm::vec3(0, -99, 350);
    static float base_scale = 1;
    static glm::vec3 base_rot = { 0, 0, 0 };
    static int v = 0;

    static const char* model_path = "E:\\AssetPack\\paladin_j_nordstrom.fbx";
    static unsigned int model_id = instance.AddAnimProp(model_path, base_loc);

    static const char* anim_path1 = "E:\\AssetPack\\Mma Kick.fbx";
    static unsigned int anim_id1 = instance.AddAnimation(anim_path1, model_id);

    static const char* anim_path2 = "E:\\AssetPack\\Standard Walk.fbx";
    static unsigned int anim_id2 = instance.AddAnimation(anim_path2, model_id);

    instance.AddToImGuiUpdate([]() {
      ImGui::Begin(model_path);
      ImGui::SliderFloat3("loc", &base_loc[0], -1000.f, 2000.f);
      ImGui::SliderFloat3("rot", &base_rot[0], -3.14f, 3.14f);
      ImGui::SliderFloat("sca", &base_scale, 1.f, 200.f);
      if (ImGui::RadioButton("none", &v, 0)) { instance.SetAnimationOnAnimProp(-1, model_id); }
      if (ImGui::RadioButton("walk", &v, 1)) {
        instance.SetAnimationOnAnimProp(anim_id2, model_id);
      }
      if (ImGui::RadioButton("kick", &v, 2)) {
        instance.SetAnimationOnAnimProp(anim_id1, model_id);
      }

      ImGui::End();

      instance.MoveAnimProp(model_id, base_loc);
      instance.ScaleAnimProp(model_id, glm::vec3(base_scale));
      instance.RotateAnimProp(model_id, base_rot);
    });

  }
}





//#pragma once
//#include "../Manager.h"
//
//bool is_models_set = false;
//void setmodels() {
//  if (is_models_set)
//    return;
//  is_models_set = true;
//
//  /// <summary>
//  /// Places static ground, re-using a plane
//  /// CORNERS:
//  /// -1820,  85,  -1805
//  ///  3005,  85,  -1805
//  ///  3020,  85,   3005
//  /// -1805,  85,   3005
//  /// </summary>
//  {
//    const int half_repeat = 4;  // the number of tiles generated around the point 0,0 is double this number
//    const int plane_stride = 400;
//    for (int i = -plane_stride * half_repeat; i < plane_stride * half_repeat * 2; i += plane_stride) {
//      for (int j = -plane_stride * half_repeat; j < plane_stride * half_repeat * 2; j += plane_stride) {
//        int large_green_plane = Engine.AddProp("res/large_green_plane.obj", glm::vec3(i, -99, j));
//        auto tmp_id = Engine.AddProp("res/neon_cube.glb", glm::vec3(i, -99, j));
//
//      }
//    }
//  }
//
//
//
//  /// <summary>
//  /// Places a wall of neon cubes with physics
//  /// </summary>
//  //{
//  //  const int scale = 10;
//  //  const int tall = 10;
//  //  const int length = 10;
//  //  for (int i = 0; i < tall; i++) {
//  //    for (int j = 0; j < length; j++) {
//  //      auto tmp_id = Engine.AddProp("res/neon_cube.glb", glm::vec3(i * scale * 2.f, -99 + ((j * scale) * 2.f) + (scale/2.f), 0), glm::vec3(scale * .5f));
//  //    }
//  //  }
//  //}
//
//  /// <summary>
//  /// Animated Model Test
//  /// </summary>
//  {
//    static glm::vec3 base_loc = glm::vec3(0, -99, 350);
//    static float base_scale = 1;
//    static glm::vec3 base_rot = { 0, 0, 0 };
//    static int curr_anim = 1;
//
//    static const char* model_path0 = "E:\\AssetPack\\paladin_j_nordstrom.fbx";
//    static unsigned int model_id = Engine.AddAnimProp(model_path0, base_loc);
//
//    static const char* anim_path0 = "E:\\AssetPack\\Mma Kick.fbx";
//    static unsigned int anim_id0 = Engine.AddAnimation(anim_path0, model_id);
//
//    static const char* anim_path1 = "E:\\AssetPack\\Standard Walk.fbx";
//    static unsigned int anim_id1 = Engine.AddAnimation(anim_path1, model_id);
//
//    switch (curr_anim) {
//    case 1: Engine.SetAnimationOnAnimProp(anim_id0, model_id); break;
//    case 2: Engine.SetAnimationOnAnimProp(anim_id1, model_id); break;
//    default: break;
//    }
//
//    Engine.AddToImGuiUpdate([]() {
//      ImGui::Begin(model_path0);
//      ImGui::SliderFloat3("loc", &base_loc[0], -1000.f, 2000.f);
//      ImGui::SliderFloat3("rot", &base_rot[0], -3.14f, 3.14f);
//      ImGui::SliderFloat("sca", &base_scale, 1.f, 200.f);
//      if (ImGui::RadioButton("none", &curr_anim, 0)) { Engine.SetAnimationOnAnimProp(-1, model_id); }
//      if (ImGui::RadioButton("kick", &curr_anim, 1)) { Engine.SetAnimationOnAnimProp(anim_id0, model_id); }
//      if (ImGui::RadioButton("walk", &curr_anim, 2)) { Engine.SetAnimationOnAnimProp(anim_id1, model_id); }
//      ImGui::End();
//      Engine.MoveAnimProp(model_id, base_loc);
//      Engine.ScaleAnimProp(model_id, glm::vec3(base_scale));
//      Engine.RotateAnimProp(model_id, base_rot);
//    });
//  }
//}
//
//
//
//
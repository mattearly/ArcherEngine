#pragma once
#include <vector>
#include <string>
#include <ArcherEngine/Interface.h>

static class Globals {
public:
  const std::string runtime_dir = "../../RuntimeFiles/";

  // test globals
  AA::Interface g_aa_interface;
  unsigned int g_imgui_func = 0;
  unsigned int g_begin_func = 0;
  unsigned int g_update_func = 0;
  unsigned int g_cam_id = 0;
  std::weak_ptr<AA::Camera> g_camera_ref;
  std::weak_ptr<AA::Window> g_window_ref;

  // resources
  // skymap files storage
  std::vector<std::string> NightSkyTextures;
  std::vector<std::string> DaySkyTextures;
  std::vector<std::string> CaveTextures;

  // model paths
  // test glb
  const std::string cube_runtime_dir = runtime_dir + "3dmodels/cube.glb";
  // test obj
  const std::string ground_plane_runtime_dir_path = runtime_dir + "3dmodels/large_green_plane.obj";
  const std::string fireplace_room_runtime_dir_path = runtime_dir + "3dmodels/fireplace_room.obj";
  // test fbx
  const std::string peasant_man_runtime_dir_path = runtime_dir + "3dmodels/peasant_man.fbx";
  const std::string peasant_girl_runtime_dir_path = runtime_dir + "3dmodels/peasant_girl.fbx";
  const std::string zombie_runtime_dir_path = runtime_dir + "3dmodels/zombie_punching.fbx"; // contains a model and animation data
  const std::string vanguard_runtime_dir_path = runtime_dir + "3dmodels/vanguard_neutral_idle.fbx"; // contains a model and animation data
  // test fbx animation
  const std::string idle_anim_runtime_dir_path = runtime_dir + "3dmodels/animations/breathing_idle.fbx"; // contains animation data


  unsigned int se_coins1 = 0;
  unsigned int se_flashlight = 0;
  unsigned int se_gunshot = 0;
  unsigned int se_mgsalert = 0;
  const std::string path_se_coins1 = runtime_dir + "soundeffects/coins.ogg";
  const std::string path_se_flashlightclick = runtime_dir + "soundeffects/flashlightclick.wav";
  const std::string path_se_gunshot = runtime_dir + "soundeffects/GunShotSingle.wav";
  const std::string path_se_mgsalert = runtime_dir + "soundeffects/Metal Gear Solid Alert.ogg";

  unsigned int g_untextured_cube_id[3] = { 0,0,0 };
  unsigned int g_ground_plane_id = 0;
  unsigned int g_peasant_man_id = 0;
  unsigned int g_peasant_girl_id = 0;
  unsigned int g_vanguard_id = 0;

  unsigned int g_zombie_id[2] = { 0, 0 };
  unsigned int g_punching_anim_id = 0;

  unsigned int g_idle_anim_id = 0;

  bool g_Yes = false, g_No = false;

  unsigned int g_plight1_id = 0;
  float  point_light_loc[3] = { -194.0f, 125.0f, -32.0f };
  float* point_light_constant = new float(1.0f);
  float* point_light_linear = new float(0.027f);
  float* point_light_quadratic = new float(0.0028f);
  float* point_light_ambient = new float(0.3f);
  float* point_light_diff = new float(0.5f);
  float* point_light_spec = new float(0.3f);
  bool debug_point_light = true;

  unsigned int g_slight1_id = 0;
  float  spot_light_loc[3] = { 0.0f, 0.0f, 0.0f };
  float  spot_light_dir[3] = { 1.f, 1.f, 1.f };
  float* spot_light_inner = new float(0.05f);
  float* spot_light_outer = new float(1.50f);
  float* spot_light_constant = new float(3.0f);
  float* spot_light_linear = new float(0.0027f);
  float* spot_light_quadratic = new float(0.00028f);
  float* spot_light_ambient = new float(0.0f);
  float* spot_light_diff = new float(1.0f);
  float* spot_light_spec = new float(1.0f);

  float* cam_fov = new float(75.f);

  bool gamma_correction = false;
} *tg;

namespace TestGlobals {

static void init() {
  if (!tg) {
    tg = new Globals();
  }
}

static void reset() {
  if (tg) {
    delete tg;
    tg = nullptr;
  }
}

}
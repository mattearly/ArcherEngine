#pragma once
#include <vector>
#include <string>
#include <AAEngine/Interface.h>

static class Globals {
public:
  const std::string runtime_dir = "../../RuntimeFiles/";

  // test globals
  AA::Interface g_aa_interface;
  unsigned int g_imgui_func = 0;
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
  const std::string fullcubepath = runtime_dir + "3dmodels/cube.glb";
  const std::string fullgroundplane = runtime_dir + "3dmodels/large_green_plane.obj";
  const std::string fullpeasant_man = runtime_dir + "3dmodels/peasant_man.dae";
  const std::string fullzombie_ = runtime_dir + "3dmodels/Zombie Punching.fbx"; // contains a model and animation data
  const std::string fullwalking_man = runtime_dir + "3dmodels/Walking.dae";  // contains a model and animation data

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

  unsigned int g_zombie_id[2] = { 0, 0 };
  unsigned int g_punching_anim_id = 0;

  unsigned int g_walking_man_id = 0;
  unsigned int g_walking_anim_id = 0;

  bool g_Yes = false, g_No = false;

  float  dir_light_direction[3] = { -0.33f, -1.0f, -0.33f };
  float* dir_light_diff = new float(0.63f);
  float* dir_light_amb = new float(0.0633f);
  float* dir_light_spec = new float(0.0653f);
  bool sun_shadows = true;

  unsigned int g_plight1_id = 0;
  float  point_light_loc[3] = { 0.0f, 0.0f, 0.0f };
  float* point_light_constant = new float(1.0f);
  float* point_light_linear = new float(0.027f);
  float* point_light_quadratic = new float(0.0028f);
  float* point_light_ambient = new float(.3f);
  float* point_light_diff = new float(.5f);
  float* point_light_spec = new float(.3f);
  bool debug_point_light = false;

  unsigned int g_slight1_id = 0;
  float  spot_light_loc[3] = { 0.0f, 0.0f, 0.0f };
  float  spot_light_dir[3] = { 1.f, 1.f, 1.f };
  float* spot_light_inner = new float(1.2f);
  float* spot_light_outer = new float(6.0f);
  float* spot_light_constant = new float(1.0f);
  float* spot_light_linear = new float(0.027f);
  float* spot_light_quadratic = new float(0.0028f);
  float* spot_light_ambient = new float(1.0f);
  float* spot_light_diff = new float(1.0f);
  float* spot_light_spec = new float(1.0f);

  float* cam_fov = new float(45.f);

  bool gamma_correction = false;
} *tg;

namespace TestGlobals {
static void init() { if (!tg) tg = new Globals(); };
static void reset() {
  if (tg) { delete tg; tg = nullptr; }
}
}
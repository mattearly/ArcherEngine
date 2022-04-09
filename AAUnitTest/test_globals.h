#pragma once
#include <vector>
#include <string>
#include <AAEngine/Interface.h>

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
const std::string fullzombie_ = runtime_dir + "3dmodels/Zombie Punching.fbx";
const std::string fullwalking_man = runtime_dir + "3dmodels/Walking.dae";

unsigned int se_coins1(0);
unsigned int se_flashlight(0);
unsigned int se_gunshot(0);
unsigned int se_mgsalert(0);
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

bool g_Yes(false), g_No(false);

float  dir_light_direction[3] = { 0.33f, -0.33f, -0.33f };
float* dir_light_diff         = new float(0.43f);
float* dir_light_amb          = new float(0.0333f);
float* dir_light_spec         = new float(0.0153f);

unsigned int g_plight1_id     = 0;
float  point_light_loc[3]     = { 0.0f, 0.0f, 0.0f };
float* point_light_constant   = new float(1.0f);
float* point_light_linear     = new float(0.027f);
float* point_light_quadratic  = new float(0.0028f);
float* point_light_ambient    = new float(.3f);
float* point_light_diff       = new float(.5f);
float* point_light_spec       = new float(.3f);


unsigned int g_slight1_id     = 0;
float  spot_light_loc[3]     = { 0.0f, 0.0f, 0.0f };
float  spot_light_dir[3] = { 1.f, 1.f, 1.f };
float* spot_light_inner   = new float(.3f);
float* spot_light_outer   = new float(.7f);
float* spot_light_constant   = new float(1.0f);
float* spot_light_linear     = new float(0.027f);
float* spot_light_quadratic  = new float(0.0028f);
float* spot_light_ambient    = new float(.3f);
float* spot_light_diff       = new float(.5f);
float* spot_light_spec       = new float(.3f);

float* cam_fov = new float(45.f);

void reset_test_globals() {

  g_imgui_func = g_update_func = 0;
  g_untextured_cube_id[0] = g_untextured_cube_id[1] = g_untextured_cube_id[2] = 0;
  g_zombie_id[0] = g_zombie_id[1] = 0;
  g_cam_id = g_ground_plane_id = g_peasant_man_id = g_walking_man_id = 0;
  g_punching_anim_id = g_walking_anim_id = 0;
  se_coins1 = se_flashlight = se_gunshot = se_mgsalert = 0;
  g_Yes = g_No = false;
  g_camera_ref.reset();
  g_window_ref.reset();
  NightSkyTextures.clear();
  DaySkyTextures.clear();
  CaveTextures.clear();

  dir_light_direction[0] = 0.33f;
  dir_light_direction[1] = -0.33f;
  dir_light_direction[2] = -0.33f;

  *dir_light_diff = 0.43f;
  *dir_light_amb = 0.0333f;
  *dir_light_spec = 0.0153f;

  g_plight1_id = 0;
  point_light_loc[0] = 0.0f;
  point_light_loc[1] = 0.0f;
  point_light_loc[2] = 0.0f;
  *point_light_constant = 1.0f;
  *point_light_linear = 0.027f;
  *point_light_quadratic = 0.0028f;
  *point_light_ambient = .3f;
  *point_light_diff = .5f;
  *point_light_spec = .3f;

  g_slight1_id = 0;
   spot_light_loc[0] = 0.f;
   spot_light_loc[1] = 0.f;
   spot_light_loc[2] = 0.f;
   spot_light_dir[0] = 1.f;
   spot_light_dir[1] = 1.f;
   spot_light_dir[2] = 1.f;
   *spot_light_inner = .3f;
   *spot_light_outer = .7f;
  *spot_light_constant  = 1.0f;
  *spot_light_linear    = 0.027f;
  *spot_light_quadratic = 0.0028f;
  *spot_light_ambient   = .3f;
  *spot_light_diff      = .5f;
  *spot_light_spec      = .3f;

  *cam_fov = 45.f;
}

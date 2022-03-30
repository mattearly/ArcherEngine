/*
* Tests some of the core functionality of the engine.
* If this fails to load resources, be sure you have ran the DownloadTestResources utility 
*   and extra to the RuntimeFiles directory.
*/

#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

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
const char* cubepath = "3dmodels/cube.glb";
std::string fullcubepath = runtime_dir + cubepath;
const char* groundplane = "3dmodels/large_green_plane.obj";
std::string fullgroundplane = runtime_dir + groundplane;
const char* peasant_man = "3dmodels/peasant_man.dae";
std::string fullpeasant_man = runtime_dir + peasant_man;
const char* zombie_ = "3dmodels/Zombie Punching.fbx";
std::string fullzombie_ = runtime_dir + zombie_;
const char* walking_man = "3dmodels/Walking.dae";
std::string fullwalking_man = runtime_dir + walking_man;

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

unsigned int g_zombie_id = 0;
unsigned int g_punching_anim_id = 0;

unsigned int g_walking_man_id  = 0;
unsigned int g_walking_anim_id = 0;

bool g_Yes(false), g_No(false);

float dir_light_direction[3] = { -0.095f, .71f, -.2f };
float* dir_light_amb = new float(.35f);
float* dir_light_diff = new float(.58f);
float* dir_light_spec = new float(.47f);

void reset_test_globals() {
  g_imgui_func = g_update_func = 0;
  g_untextured_cube_id[0] = g_untextured_cube_id[1] = g_untextured_cube_id[2] = 0;
  g_cam_id = g_ground_plane_id = g_peasant_man_id = g_zombie_id = g_walking_man_id = 0;
  g_punching_anim_id = g_walking_anim_id = 0;
  se_coins1 = se_flashlight = se_gunshot = se_mgsalert = 0;
  g_Yes = g_No = false;
  g_camera_ref.reset();
  g_window_ref.reset();
  NightSkyTextures.clear();
  DaySkyTextures.clear();
  CaveTextures.clear();
  dir_light_direction[0] = -0.095f;
  dir_light_direction[1] = 0.71f;
  dir_light_direction[2] = -0.2f;
  *dir_light_amb = 0.35f;
  *dir_light_diff = 0.58f;
  *dir_light_spec = 0.47f;
}

// functions to toggle fly controls
extern void setup_fpp_fly(unsigned int);
extern void turn_off_fly();

namespace AAUnitTest
{
TEST_CLASS(AAUnitTest) {
public:

  TEST_METHOD(DefaultInit) {
    bool initSuccess = g_aa_interface.Init();
    Assert::AreEqual(initSuccess, true);
    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("default init");
      ImGui::Text("Click \"ok\" to continue");
      bool result = ImGui::Button("ok");
      ImGui::End();

      // update state
      if (result) { g_aa_interface.Shutdown(); };
      });
    int run_diag = g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    reset_test_globals();
  }
  TEST_METHOD(WindowOptions) {
    {
      AA::WindowOptions winopts;
      winopts._vsync = true;
      winopts._title = "WindowOptions";
      winopts._cursor_mode = AA::CURSOR_MODE::NORMAL;
      winopts._height = -1;  // should enfore min size instead of -1
      winopts._width = -1;
      winopts._min_width = 500;  // should enfore this instead of engine const _MIN_SIZE
      winopts._min_height = 500;
      bool initSuccess = g_aa_interface.Init(winopts);
      Assert::AreEqual(initSuccess, true);
    }

    static int min_w_h[2] = { 0,0 };

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Options Test");
      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      bool WinSizeIncr = ImGui::Button("+Width/Height");
      bool WinSizeDecr = ImGui::Button("-Width/Height");
      bool MinSizeChanged = ImGui::InputInt2("MinWindowWidthHeight", min_w_h);
      ImGui::Text("Does Everything Work As Expected?");
      g_Yes = ImGui::Button("Yes");
      g_No = ImGui::Button("No");
      ImGui::End();

      // update state
      if (WinSizeIncr) {
        g_window_ref = g_aa_interface.GetWindow();
        std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();
        int new_width = local_window_ref->GetCurrentWidth() + 50;
        int new_height = local_window_ref->GetCurrentHeight() + 50;
        local_window_ref->SetNewWidthAndHeight(new_width, new_height);
        Assert::AreEqual(local_window_ref->GetCurrentWidth(), new_width);
        Assert::AreEqual(local_window_ref->GetCurrentHeight(), new_height);
      }
      if (doToggleFS) { g_aa_interface.ToggleWindowFullscreen(); };
      if (!MinSizeChanged) {
        g_window_ref = g_aa_interface.GetWindow();
        std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();
        local_window_ref->SetNewMinWidthAndHeight(min_w_h[0], min_w_h[1]);
        min_w_h[0] = local_window_ref->GetCurrentMinWidth();
        min_w_h[1] = local_window_ref->GetCurrentMinHeight();
      }
      if (WinSizeDecr) {
        g_window_ref = g_aa_interface.GetWindow();
        std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock(); 
        int new_width = local_window_ref->GetCurrentWidth() - 50;
        int new_height = local_window_ref->GetCurrentHeight() - 50;
        local_window_ref->SetNewWidthAndHeight(new_width, new_height);
      }
      if (g_Yes || g_No) { g_aa_interface.Shutdown(); };
      });

    int run_diag = g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(g_No, false);

    reset_test_globals();
  }

  TEST_METHOD(Skybox) {
    // load skymap files
    {
      // night time skybox files
      const std::string skyboxfolder = runtime_dir + "skybox/night/";
      const std::string order[6] = { "right",  "left",  "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      NightSkyTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        NightSkyTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    {
      // day time skybox files
      const std::string skyboxfolder = runtime_dir + "skybox/day/elyvisions/";
      const std::string order[6] = { "right",  "left",  "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      DaySkyTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        DaySkyTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    {
      // cave skybox files (with no alpha channel)
      const std::string skyboxfolder = runtime_dir + "skybox/cave/elyvisions/";
      const std::string order[6] = { "right",  "left",  "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      CaveTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        CaveTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    // init engine
    bool initSuccess = g_aa_interface.Init();
    Assert::AreEqual(initSuccess, true);

    // add a camera that fills full screens
    {
      g_window_ref = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();
      g_cam_id = g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      g_camera_ref = g_aa_interface.GetCamera(g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
    }

    g_update_func = g_aa_interface.AddToUpdate([](float dt) {
      static float accum_time = 0.f;
      accum_time += dt;
      g_window_ref = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Camera> local_camera_ref = g_camera_ref.lock();
      local_camera_ref->ShiftPitchAndYaw(0, dt * 2);
      local_camera_ref->SetPitch(sin(accum_time) * 2);
      });

    static bool UserSaidSkyboxFailed = false;

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Skybox Test");
      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      if (doToggleFS) {
        g_aa_interface.ToggleWindowFullscreen();
      };
      bool Day = ImGui::Button("Switch To Day Skybox");
      bool Night = ImGui::Button("Switch To Night Skybox");
      bool Cave = ImGui::Button("Switch To Cave Skybox");
      ImGui::Spacing();
      ImGui::Text("Do All Skyboxes Work As Expected?");
      bool Done = ImGui::Button("Yes");
      UserSaidSkyboxFailed = ImGui::Button("No");
      ImGui::End();
      if (Night) {
        g_aa_interface.SetSkybox(NightSkyTextures);
      } else if (Day) {
        g_aa_interface.SetSkybox(DaySkyTextures);
      } else if (Cave) {
        g_aa_interface.SetSkybox(CaveTextures);
      } else if (Done || UserSaidSkyboxFailed) {
        g_aa_interface.Shutdown();
      };
      });

    int run_diag = g_aa_interface.Run();

    Assert::AreEqual(run_diag, 0);

    Assert::AreEqual(UserSaidSkyboxFailed, false);

    reset_test_globals();
  }

  TEST_METHOD(Models) {
    // init engine
    {
      bool initSuccess = g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    // camera that stays screen size
    {
      g_window_ref = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();
      g_cam_id = g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      g_camera_ref = g_aa_interface.GetCamera(g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(75.f);
      setup_fpp_fly(g_cam_id);
    }

    // load models
    g_untextured_cube_id[0] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(-20, 0, -25));
    g_ground_plane_id = g_aa_interface.AddProp(fullgroundplane.c_str(), glm::vec3(0, -30.f, 0), glm::vec3(2));
    g_peasant_man_id = g_aa_interface.AddProp(fullpeasant_man.c_str(), glm::vec3(0, -30, -100), glm::vec3(.25f));

    g_update_func = g_aa_interface.AddToUpdate([](float dt) {
      static float accum_time = 0;
      accum_time += dt;
      g_aa_interface.RotateProp(g_untextured_cube_id[0], glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      });

    // default light and background
    g_aa_interface.SetWindowClearColor();

    g_aa_interface.SetDirectionalLight(
      glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
      glm::vec3(*dir_light_amb),
      glm::vec3(*dir_light_diff),
      glm::vec3(*dir_light_spec));

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Model Test");
      bool update_light1 = ImGui::SliderFloat3("Light Direction", dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_light2 = ImGui::SliderFloat("Light Ambient", dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_light3 = ImGui::SliderFloat("Light Diffuse", dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_light4 = ImGui::SliderFloat("Light Spec", dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      g_Yes = ImGui::Button("Yes");
      g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_light1 || update_light2 || update_light3 || update_light4)
        g_aa_interface.SetDirectionalLight(
          glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
          glm::vec3(*dir_light_amb),
          glm::vec3(*dir_light_diff),
          glm::vec3(*dir_light_spec));
      if (g_Yes || g_No) { g_aa_interface.Shutdown(); };
      });

    g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });

    int run_diag = g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(g_No, false);

    reset_test_globals();
  }

  TEST_METHOD(SkeletalAnimation) {
    {
      bool initSuccess = g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    // camera that stays screen size
    {
      g_window_ref = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();
      g_cam_id = g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      g_camera_ref = g_aa_interface.GetCamera(g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(75.f);

      setup_fpp_fly(g_cam_id);
    }

    g_zombie_id = g_aa_interface.AddAnimProp(fullzombie_.c_str(), glm::vec3(0, -30, -75), glm::vec3(.25f));
    g_punching_anim_id = g_aa_interface.AddAnimation(fullzombie_.c_str(), g_zombie_id);
    g_aa_interface.SetAnimationOnAnimProp(g_punching_anim_id, g_zombie_id);

    // default light and background
    g_aa_interface.SetWindowClearColor();

    g_aa_interface.SetDirectionalLight(
      glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
      glm::vec3(*dir_light_amb),
      glm::vec3(*dir_light_diff),
      glm::vec3(*dir_light_spec));

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Animated Model Test");
      bool update_light1 = ImGui::SliderFloat3("Light Direction", dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_light2 = ImGui::SliderFloat("Light Ambient", dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_light3 = ImGui::SliderFloat("Light Diffuse", dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_light4 = ImGui::SliderFloat("Light Spec", dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      g_Yes = ImGui::Button("Yes");
      g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_light1 || update_light2 || update_light3 || update_light4)
        g_aa_interface.SetDirectionalLight(
          glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
          glm::vec3(*dir_light_amb),
          glm::vec3(*dir_light_diff),
          glm::vec3(*dir_light_spec));
      if (g_Yes || g_No) { g_aa_interface.Shutdown(); };
      });

    g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });

    int run_diag = g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(g_No, false);
    reset_test_globals();
  }

  TEST_METHOD(ModelMashup) {
    // init AA
    {
      bool initSuccess = g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    // camera that stays screen size
    {
      g_window_ref = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();
      g_cam_id = g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      g_camera_ref = g_aa_interface.GetCamera(g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(75.f);
      setup_fpp_fly(g_cam_id);
    }

    // load models
    g_untextured_cube_id[0] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(-10, -10, -10));
    g_ground_plane_id = g_aa_interface.AddProp(fullgroundplane.c_str(), glm::vec3(0, -30.f, 0), glm::vec3(2));

    g_peasant_man_id = g_aa_interface.AddProp(fullpeasant_man.c_str(), glm::vec3(0, -30, -70), glm::vec3(.15f));

    g_zombie_id = g_aa_interface.AddAnimProp(fullzombie_.c_str(), glm::vec3(-30, -30, -70), glm::vec3(.15f));
    g_punching_anim_id = g_aa_interface.AddAnimation(fullzombie_.c_str(), g_zombie_id);
    g_aa_interface.SetAnimationOnAnimProp(g_punching_anim_id, g_zombie_id);

    g_walking_man_id = g_aa_interface.AddAnimProp(fullwalking_man.c_str(), glm::vec3(30, -30, -70), glm::vec3(3));
    g_walking_anim_id = g_aa_interface.AddAnimation(fullwalking_man.c_str(), g_walking_man_id);
    g_aa_interface.SetAnimationOnAnimProp(g_walking_anim_id, g_walking_man_id);

    // default light and background
    g_aa_interface.SetWindowClearColor();

    g_aa_interface.SetDirectionalLight(
      glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
      glm::vec3(*dir_light_amb),
      glm::vec3(*dir_light_diff),
      glm::vec3(*dir_light_spec));

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Model Mashup Test");
      bool update_light1 = ImGui::SliderFloat3("Light Direction", dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_light2 = ImGui::SliderFloat("Light Ambient", dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_light3 = ImGui::SliderFloat("Light Diffuse", dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_light4 = ImGui::SliderFloat("Light Spec", dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      g_Yes = ImGui::Button("Yes");
      g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_light1 || update_light2 || update_light3 || update_light4)
        g_aa_interface.SetDirectionalLight(
          glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
          glm::vec3(*dir_light_amb),
          glm::vec3(*dir_light_diff),
          glm::vec3(*dir_light_spec));
      if (g_Yes || g_No) { g_aa_interface.Shutdown(); };
      });

    g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });

    int run_diag = g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(g_No, false);
    reset_test_globals();
  }

  TEST_METHOD(StencilOutline) {
    // init engine
    {
      bool initSuccess = g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    // camera that stays screen size
    {
      g_window_ref = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();
      g_cam_id = g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      g_camera_ref = g_aa_interface.GetCamera(g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(75.f);
      setup_fpp_fly(g_cam_id);
    }

    // load models
    g_untextured_cube_id[0] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(0, 0, -10));
    g_aa_interface.StencilProp(g_untextured_cube_id[0], true);
    g_aa_interface.StencilPropColor(g_untextured_cube_id[0], glm::vec3(.4, .4, .4));
    g_aa_interface.StencilPropWithNormals(g_untextured_cube_id[0], false);
    g_aa_interface.StencilPropScale(g_untextured_cube_id[0], 1.1f);

    g_untextured_cube_id[1] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(-10, 0, -10));
    g_untextured_cube_id[2] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(10, 0, -10));

    g_ground_plane_id = g_aa_interface.AddProp(fullgroundplane.c_str(), glm::vec3(0, -30.f, 0), glm::vec3(3, 1, 3));

    g_peasant_man_id = g_aa_interface.AddProp(fullpeasant_man.c_str(), glm::vec3(0, -30, -100), glm::vec3(.25f));
    g_aa_interface.StencilProp(g_peasant_man_id, true);
    g_aa_interface.StencilPropColor(g_peasant_man_id, glm::vec3(.4, .4, .4));
    g_aa_interface.StencilPropWithNormals(g_peasant_man_id, true);
    g_aa_interface.StencilPropScale(g_peasant_man_id, 3.f);

    g_walking_man_id = g_aa_interface.AddProp(fullwalking_man.c_str(), glm::vec3(-60, -30, -100), glm::vec3(.25f));
    g_aa_interface.StencilProp(g_walking_man_id, true);
    g_aa_interface.StencilPropColor(g_walking_man_id, glm::vec3(.4, .4, .4));
    g_aa_interface.StencilPropWithNormals(g_walking_man_id, true);
    g_aa_interface.StencilPropScale(g_walking_man_id, 3.f);

    g_aa_interface.AddToUpdate([](float dt) {
      static float accum_time = 0;
      accum_time += dt;
      g_aa_interface.RotateProp(g_untextured_cube_id[0], glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      g_aa_interface.RotateProp(g_untextured_cube_id[1], glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      g_aa_interface.RotateProp(g_untextured_cube_id[2], glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      });

    // default light and background
    g_aa_interface.SetWindowClearColor();
    g_aa_interface.SetDirectionalLight(
      glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
      glm::vec3(*dir_light_amb),
      glm::vec3(*dir_light_diff),
      glm::vec3(*dir_light_spec));

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Model Mashup Test");
      bool update_light1 = ImGui::SliderFloat3("Light Direction", dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_light2 = ImGui::SliderFloat("Light Ambient", dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_light3 = ImGui::SliderFloat("Light Diffuse", dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_light4 = ImGui::SliderFloat("Light Spec", dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      g_Yes = ImGui::Button("Yes");
      g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_light1 || update_light2 || update_light3 || update_light4)
        g_aa_interface.SetDirectionalLight(
          glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
          glm::vec3(*dir_light_amb),
          glm::vec3(*dir_light_diff),
          glm::vec3(*dir_light_spec));
      if (g_Yes || g_No) { g_aa_interface.Shutdown(); };
      });

    int run_diag = g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(g_No, false);
    reset_test_globals();
  }

  TEST_METHOD(SoundEffects) {

    {
      // asset paths

      bool initSuccess = g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);

      se_coins1 = g_aa_interface.AddSoundEffect(path_se_coins1.c_str());
      se_flashlight = g_aa_interface.AddSoundEffect(path_se_flashlightclick.c_str());
      se_gunshot = g_aa_interface.AddSoundEffect(path_se_gunshot.c_str());
      se_mgsalert = g_aa_interface.AddSoundEffect(path_se_mgsalert.c_str());
    }

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Sound Effect Test");
      bool s1 = ImGui::Button("Play Coin Sound 1 (.ogg)");
      bool s2 = ImGui::Button("Play Flashlight Sound (.wav)");
      bool s3 = ImGui::Button("Play Gunshot Sound (.wav)");
      bool s4 = ImGui::Button("Play MGS Alert Sound (.ogg)");

      ImGui::Text("Do all sound effects work?");
      g_Yes = ImGui::Button("Yes");
      g_No = ImGui::Button("No");
      ImGui::End();

      // update state
      if (s1) g_aa_interface.PlaySoundEffect(se_coins1);
      if (s2) g_aa_interface.PlaySoundEffect(se_flashlight);
      if (s3) g_aa_interface.PlaySoundEffect(se_gunshot);
      if (s4) g_aa_interface.PlaySoundEffect(se_mgsalert);

      if (g_Yes || g_No) { g_aa_interface.Shutdown(); };
      });

    int run_diag = g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(g_No, false);
    reset_test_globals();
  }



  // todo: reuse model tests

  // todo: reuse anim model tests

  // todo: music tests

};
}

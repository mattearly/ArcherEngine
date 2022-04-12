/*
* Tests some of the core functionality of the engine.
* If this fails to load resources, be sure you have ran the DownloadTestResources utility
*   and extra to the RuntimeFiles directory.
*/

#include "pch.h"
#include "test_globals.h"
#include "fly_cam_script.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AAUnitTest
{
TEST_CLASS(AAUnitTest) {
public:

  TEST_METHOD(DefaultInit) {
    TestGlobals::init();
    bool initSuccess = tg->g_aa_interface.Init();
    Assert::AreEqual(initSuccess, true);
    tg->g_imgui_func = tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("default init");
      ImGui::Text("Click \"ok\" to continue");
      bool result = ImGui::Button("ok");
      ImGui::End();

      // update state
      if (result) { tg->g_aa_interface.Shutdown(); };
      });
    int run_diag = tg->g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    TestGlobals::reset();
  }

  TEST_METHOD(WindowOptions) {
    TestGlobals::init();
    {
      AA::WindowOptions winopts;
      winopts._vsync = true;
      winopts._title = "WindowOptions";
      winopts._cursor_mode = AA::CURSOR_MODE::NORMAL;
      winopts._height = -1;  // should enfore min size instead of -1
      winopts._width = -1;
      winopts._min_width = 500;  // should enfore this instead of engine const _MIN_SIZE
      winopts._min_height = 500;
      bool initSuccess = tg->g_aa_interface.Init(winopts);
      Assert::AreEqual(initSuccess, true);
    }

    static int min_w_h[2] = { 0,0 };

    tg->g_imgui_func = tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Options Test");
      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      bool WinSizeIncr = ImGui::Button("+Width/Height");
      bool WinSizeDecr = ImGui::Button("-Width/Height");
      bool MinSizeChanged = ImGui::InputInt2("MinWindowWidthHeight", min_w_h);
      ImGui::Text("Does Everything Work As Expected?");
      tg->g_Yes = ImGui::Button("Yes");
      tg->g_No = ImGui::Button("No");
      ImGui::End();

      // update state
      if (WinSizeIncr) {
        tg->g_window_ref = tg->g_aa_interface.GetWindow();
        std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();
        int new_width = local_window_ref->GetCurrentWidth() + 50;
        int new_height = local_window_ref->GetCurrentHeight() + 50;
        local_window_ref->SetNewWidthAndHeight(new_width, new_height);
        Assert::AreEqual(local_window_ref->GetCurrentWidth(), new_width);
        Assert::AreEqual(local_window_ref->GetCurrentHeight(), new_height);
      }
      if (doToggleFS) { tg->g_aa_interface.ToggleWindowFullscreen(); };
      if (!MinSizeChanged) {
        tg->g_window_ref = tg->g_aa_interface.GetWindow();
        std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();
        local_window_ref->SetNewMinWidthAndHeight(min_w_h[0], min_w_h[1]);
        min_w_h[0] = local_window_ref->GetCurrentMinWidth();
        min_w_h[1] = local_window_ref->GetCurrentMinHeight();
      }
      if (WinSizeDecr) {
        tg->g_window_ref = tg->g_aa_interface.GetWindow();
        std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();
        int new_width = local_window_ref->GetCurrentWidth() - 50;
        int new_height = local_window_ref->GetCurrentHeight() - 50;
        local_window_ref->SetNewWidthAndHeight(new_width, new_height);
      }
      if (tg->g_Yes || tg->g_No) { tg->g_aa_interface.Shutdown(); };
      });

    int run_diag = tg->g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(tg->g_No, false);

    TestGlobals::reset();
  }

  TEST_METHOD(Skybox) {
    TestGlobals::init();
    // load skymap files
    {
      // night time skybox files
      const std::string skyboxfolder = tg->runtime_dir + "skybox/night/";
      const std::string order[6] = { "right",  "left",  "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      tg->NightSkyTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        tg->NightSkyTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    {
      // day time skybox files
      const std::string skyboxfolder = tg->runtime_dir + "skybox/day/elyvisions/";
      const std::string order[6] = { "right",  "left",  "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      tg->DaySkyTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        tg->DaySkyTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    {
      // cave skybox files (with no alpha channel)
      const std::string skyboxfolder = tg->runtime_dir + "skybox/cave/elyvisions/";
      const std::string order[6] = { "right",  "left",  "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      tg->CaveTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        tg->CaveTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    // init engine
    bool initSuccess = tg->g_aa_interface.Init();
    Assert::AreEqual(initSuccess, true);

    // add a camera that fills full screens
    {
      tg->g_window_ref = tg->g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();
      tg->g_cam_id = tg->g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      tg->g_camera_ref = tg->g_aa_interface.GetCamera(tg->g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = tg->g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
    }

    tg->g_update_func = tg->g_aa_interface.AddToUpdate([](float dt) {
      static float accum_time = 0.f;
      accum_time += dt;
      tg->g_window_ref = tg->g_aa_interface.GetWindow();
      std::shared_ptr<AA::Camera> local_camera_ref = tg->g_camera_ref.lock();
      local_camera_ref->ShiftPitchAndYaw(0, dt * 2);
      local_camera_ref->SetPitch(sin(accum_time) * 2);
      });

    static bool UserSaidSkyboxFailed = false;

    tg->g_imgui_func = tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Skybox Test");
      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      if (doToggleFS) {
        tg->g_aa_interface.ToggleWindowFullscreen();
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
        std::shared_ptr<AA::Camera> night_cam = tg->g_camera_ref.lock();
        night_cam->SetSkybox(tg->NightSkyTextures);
      } else if (Day) {
        std::shared_ptr<AA::Camera> day_cam = tg->g_camera_ref.lock();
        day_cam->SetSkybox(tg->DaySkyTextures);
      } else if (Cave) {
        std::shared_ptr<AA::Camera> cave_cam = tg->g_camera_ref.lock();
        cave_cam->SetSkybox(tg->CaveTextures);
      } else if (Done || UserSaidSkyboxFailed) {
        tg->g_aa_interface.Shutdown();
      };
      });

    int run_diag = tg->g_aa_interface.Run();

    Assert::AreEqual(run_diag, 0);

    Assert::AreEqual(UserSaidSkyboxFailed, false);

    TestGlobals::reset();
  }

  TEST_METHOD(Models) {
    TestGlobals::init();

    // init engine
    {
      bool initSuccess = tg->g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    // camera that stays screen size
    {
      tg->g_window_ref = tg->g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();
      tg->g_cam_id = tg->g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      tg->g_camera_ref = tg->g_aa_interface.GetCamera(tg->g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = tg->g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(75.f);
      setup_fpp_fly(tg->g_cam_id, tg->g_aa_interface);
    }

    // load models
    tg->g_untextured_cube_id[0] = tg->g_aa_interface.AddProp(tg->fullcubepath.c_str(), glm::vec3(-20, 0, -25));
    tg->g_ground_plane_id = tg->g_aa_interface.AddProp(tg->fullgroundplane.c_str(), glm::vec3(0, -30.f, 0), glm::vec3(2));
    tg->g_peasant_man_id = tg->g_aa_interface.AddProp(tg->fullpeasant_man.c_str(), glm::vec3(0, -30, -100), glm::vec3(.25f));

    tg->g_update_func = tg->g_aa_interface.AddToUpdate([](float dt) {
      static float accum_time = 0;
      accum_time += dt;
      auto t1 = tg->g_aa_interface.GetProp(tg->g_untextured_cube_id[0]);
      std::shared_ptr<AA::Prop> s1 = t1.lock();
      s1->SetRotation(glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      });

    // default light and background
    tg->g_aa_interface.SetWindowClearColor();

    tg->g_aa_interface.SetDirectionalLight(
      glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
      glm::vec3(*tg->dir_light_amb),
      glm::vec3(*tg->dir_light_diff),
      glm::vec3(*tg->dir_light_spec));

    tg->g_imgui_func = tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Model Test");
      bool update_dlight_dir = ImGui::SliderFloat3("Light Direction", tg->dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Light Ambient", tg->dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Light Diffuse", tg->dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Light Spec", tg->dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      tg->g_Yes = ImGui::Button("Yes");
      tg->g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular)
        tg->g_aa_interface.SetDirectionalLight(
          glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
          glm::vec3(*tg->dir_light_amb),
          glm::vec3(*tg->dir_light_diff),
          glm::vec3(*tg->dir_light_spec));
      if (tg->g_Yes || tg->g_No) { tg->g_aa_interface.Shutdown(); };
      });

    tg->g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });

    int run_diag = tg->g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(tg->g_No, false);

    TestGlobals::reset();
  }

  TEST_METHOD(SkeletalAnimation) {
    TestGlobals::init();

    {
      bool initSuccess = tg->g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    // camera that stays screen size
    {
      tg->g_window_ref = tg->g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();
      tg->g_cam_id = tg->g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      tg->g_camera_ref = tg->g_aa_interface.GetCamera(tg->g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = tg->g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(75.f);

      setup_fpp_fly(tg->g_cam_id, tg->g_aa_interface);
    }

    tg->g_zombie_id[0] = tg->g_aa_interface.AddAnimProp(tg->fullzombie_.c_str(), glm::vec3(0, -30, -75), glm::vec3(.25f));
    tg->g_punching_anim_id = tg->g_aa_interface.AddAnimation(tg->fullzombie_.c_str(), tg->g_zombie_id[0]);
    tg->g_aa_interface.SetAnimationOnAnimProp(tg->g_punching_anim_id, tg->g_zombie_id[0]);

    // default light and background
    tg->g_aa_interface.SetWindowClearColor();

    tg->g_aa_interface.SetDirectionalLight(
      glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
      glm::vec3(*tg->dir_light_amb),
      glm::vec3(*tg->dir_light_diff),
      glm::vec3(*tg->dir_light_spec));

    tg->g_imgui_func = tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Animated Model Test");
      bool update_dlight_dir = ImGui::SliderFloat3("Light Direction", tg->dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Light Ambient", tg->dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Light Diffuse", tg->dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Light Spec", tg->dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      tg->g_Yes = ImGui::Button("Yes");
      tg->g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular)
        tg->g_aa_interface.SetDirectionalLight(
          glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
          glm::vec3(*tg->dir_light_amb),
          glm::vec3(*tg->dir_light_diff),
          glm::vec3(*tg->dir_light_spec));
      if (tg->g_Yes || tg->g_No) { tg->g_aa_interface.Shutdown(); };
      });

    tg->g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });

    int run_diag = tg->g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(tg->g_No, false);
    TestGlobals::reset();
  }

  TEST_METHOD(SpaceSceneMashup) {
    TestGlobals::init();

    // init AA
    {
      bool initSuccess = tg->g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    tg->g_aa_interface.SetWindowTitle("Space Scene Mashup Test");

    // night time skybox files
    {
      const std::string skyboxfolder = tg->runtime_dir + "skybox/night/";
      const std::string order[6] = { "right",  "left",  "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      tg->NightSkyTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        tg->NightSkyTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    // camera that stays screen size
    {
      tg->g_window_ref = tg->g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();
      tg->g_cam_id = tg->g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());


      tg->g_camera_ref = tg->g_aa_interface.GetCamera(tg->g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = tg->g_camera_ref.lock();
      local_camera_ref->SetSkybox(tg->NightSkyTextures);
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(*tg->cam_fov);
      setup_fpp_fly(tg->g_cam_id, tg->g_aa_interface);
    }

    {
      tg->g_untextured_cube_id[0] = tg->g_aa_interface.AddProp(tg->fullcubepath.c_str(), glm::vec3(-10, -10, -10));
      auto w1 = tg->g_aa_interface.GetProp(tg->g_untextured_cube_id[0]);
      std::shared_ptr<AA::Prop> s1 = w1.lock();
      s1->SetRotation(glm::vec3(1.571f, 3.14159f, 0));
      s1->SetStencil(true);
      s1->SetStencilColor(glm::vec3(.4, .4, .4));
      s1->SetStencilWithNormals(false);
      s1->SetStencilScale(1.1f);
    }

    tg->g_ground_plane_id = tg->g_aa_interface.AddProp(tg->fullgroundplane.c_str(), glm::vec3(0, -30.f, 0), glm::vec3(3));


    //tg->g_peasant_man_id = tg->g_aa_interface.AddProp(tg->fullpeasant_man.c_str(), glm::vec3(0, -30, -70), glm::vec3(.15f));



    // Add Zombie With Punching Animation.
    tg->g_zombie_id[0] = tg->g_aa_interface.AddAnimProp(tg->fullzombie_.c_str(), glm::vec3(-30, -30, -70), glm::vec3(0.12f));
    tg->g_punching_anim_id = tg->g_aa_interface.AddAnimation(tg->fullzombie_.c_str(), tg->g_zombie_id[0]);
    tg->g_aa_interface.SetAnimationOnAnimProp(tg->g_punching_anim_id, tg->g_zombie_id[0]);
    {
      auto weak_tmp = tg->g_aa_interface.GetAnimProp(tg->g_zombie_id[0]);
      auto strong_tmp = weak_tmp.lock();
      strong_tmp->SetStencil(true);
      strong_tmp->SetStencilWithNormals(true);
      strong_tmp->SetStencilColor(glm::vec3(0, 9, 0));
      strong_tmp->SetStencilScale(5.f);

    }




    // default light and background
    //tg->g_aa_interface.SetWindowClearColor();

    tg->g_aa_interface.SetDirectionalLight(
      glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
      glm::vec3(*tg->dir_light_amb),
      glm::vec3(*tg->dir_light_diff),
      glm::vec3(*tg->dir_light_spec));

    tg->g_imgui_func = tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Space Scene Mashup");
      ImGui::Text("Directional Light Controls");
      bool update_dlight_dir = ImGui::SliderFloat3("Direction", tg->dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Diffuse", tg->dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Ambient", tg->dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Specular", tg->dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Camera Controls");
      bool update_cam_fov = ImGui::SliderFloat("FOV", tg->cam_fov, 30.f, 90.f);
      tg->g_No = ImGui::Button("report broken");
      tg->g_Yes = ImGui::Button("QUIT");
      ImGui::End();

      // state update
      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular)
        tg->g_aa_interface.SetDirectionalLight(
          glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
          glm::vec3(*tg->dir_light_amb),
          glm::vec3(*tg->dir_light_diff),
          glm::vec3(*tg->dir_light_spec));
      if (update_cam_fov) {
        auto cam = tg->g_camera_ref.lock();
        cam->SetFOV(*tg->cam_fov);
      }
      if (tg->g_Yes || tg->g_No) { tg->g_aa_interface.Shutdown(); };
      });

    tg->g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });

    int run_diag = tg->g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(tg->g_No, false);
    TestGlobals::reset();
  }

  TEST_METHOD(StencilOutline) {
    TestGlobals::init();

    // init engine
    {
      bool initSuccess = tg->g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    // camera that stays screen size
    {
      tg->g_window_ref = tg->g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();
      tg->g_cam_id = tg->g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      tg->g_camera_ref = tg->g_aa_interface.GetCamera(tg->g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = tg->g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(75.f);
      setup_fpp_fly(tg->g_cam_id, tg->g_aa_interface);
    }

    // load models
    {
      tg->g_untextured_cube_id[0] = tg->g_aa_interface.AddProp(tg->fullcubepath.c_str(), glm::vec3(0, 0, -10));
      auto w1 = tg->g_aa_interface.GetProp(tg->g_untextured_cube_id[0]);
      std::shared_ptr<AA::Prop> s1 = w1.lock();
      s1->SetStencil(true);
      s1->SetStencilColor(glm::vec3(.4, .4, .4));
      s1->SetStencilWithNormals(false);
      s1->SetStencilScale(1.1f);

      tg->g_untextured_cube_id[1] = tg->g_aa_interface.AddProp(tg->fullcubepath.c_str(), glm::vec3(-10, 0, -10));
      tg->g_untextured_cube_id[2] = tg->g_aa_interface.AddProp(tg->fullcubepath.c_str(), glm::vec3(10, 0, -10));

      tg->g_ground_plane_id = tg->g_aa_interface.AddProp(tg->fullgroundplane.c_str(), glm::vec3(0, -30.f, 0), glm::vec3(3, 1, 3));

      tg->g_peasant_man_id = tg->g_aa_interface.AddProp(tg->fullpeasant_man.c_str(), glm::vec3(0, -30, -100), glm::vec3(.25f));
      auto w2 = tg->g_aa_interface.GetProp(tg->g_peasant_man_id);
      std::shared_ptr<AA::Prop> s2 = w2.lock();
      s2->SetStencil(true);
      s2->SetStencilColor(glm::vec3(.4, .4, .4));
      s2->SetStencilWithNormals(true);
      s2->SetStencilScale(3.f);

      tg->g_walking_man_id = tg->g_aa_interface.AddProp(tg->fullwalking_man.c_str(), glm::vec3(-60, -30, -100), glm::vec3(.25f));
      auto w3 = tg->g_aa_interface.GetProp(tg->g_walking_man_id);
      std::shared_ptr<AA::Prop> s3 = w3.lock();
      s3->SetStencil(true);
      s3->SetStencilColor(glm::vec3(.4, .4, .4));
      s3->SetStencilWithNormals(true);
      s3->SetStencilScale(3.f);
    }

    tg->g_aa_interface.AddToUpdate([](float dt) {
      static float accum_time = 0;
      accum_time += dt;
      auto t1 = tg->g_aa_interface.GetProp(tg->g_untextured_cube_id[0]);
      auto t2 = tg->g_aa_interface.GetProp(tg->g_untextured_cube_id[1]);
      auto t3 = tg->g_aa_interface.GetProp(tg->g_untextured_cube_id[2]);
      std::shared_ptr<AA::Prop> st1 = t1.lock();
      std::shared_ptr<AA::Prop> st2 = t2.lock();
      std::shared_ptr<AA::Prop> st3 = t3.lock();
      st1->SetRotation(glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      st2->SetRotation(glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      st3->SetRotation(glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      });

    // default light and background
    tg->g_aa_interface.SetWindowClearColor();
    tg->g_aa_interface.SetDirectionalLight(
      glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
      glm::vec3(*tg->dir_light_amb),
      glm::vec3(*tg->dir_light_diff),
      glm::vec3(*tg->dir_light_spec));

    tg->g_imgui_func = tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Stencil Outline Test");
      bool update_dlight_dir = ImGui::SliderFloat3("Light Direction", tg->dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Light Ambient", tg->dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Light Diffuse", tg->dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Light Spec", tg->dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      tg->g_Yes = ImGui::Button("Yes");
      tg->g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular)
        tg->g_aa_interface.SetDirectionalLight(
          glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
          glm::vec3(*tg->dir_light_amb),
          glm::vec3(*tg->dir_light_diff),
          glm::vec3(*tg->dir_light_spec));
      if (tg->g_Yes || tg->g_No) { tg->g_aa_interface.Shutdown(); };
      });

    int run_diag = tg->g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(tg->g_No, false);
    TestGlobals::reset();
  }

  TEST_METHOD(SoundEffects) {
    TestGlobals::init();

    {
      // asset paths

      bool initSuccess = tg->g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);

      tg->se_coins1 = tg->g_aa_interface.AddSoundEffect(tg->path_se_coins1.c_str());
      tg->se_flashlight = tg->g_aa_interface.AddSoundEffect(tg->path_se_flashlightclick.c_str());
      tg->se_gunshot = tg->g_aa_interface.AddSoundEffect(tg->path_se_gunshot.c_str());
      tg->se_mgsalert = tg->g_aa_interface.AddSoundEffect(tg->path_se_mgsalert.c_str());
    }

    tg->g_imgui_func = tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Sound Effect Test");
      bool s1 = ImGui::Button("Play Coin Sound 1 (.ogg)");
      bool s2 = ImGui::Button("Play Flashlight Sound (.wav)");
      bool s3 = ImGui::Button("Play Gunshot Sound (.wav)");
      bool s4 = ImGui::Button("Play MGS Alert Sound (.ogg)");

      ImGui::Text("Do all sound effects work?");
      tg->g_Yes = ImGui::Button("Yes");
      tg->g_No = ImGui::Button("No");
      ImGui::End();

      // update state
      if (s1) tg->g_aa_interface.PlaySoundEffect(tg->se_coins1);
      if (s2) tg->g_aa_interface.PlaySoundEffect(tg->se_flashlight);
      if (s3) tg->g_aa_interface.PlaySoundEffect(tg->se_gunshot);
      if (s4) tg->g_aa_interface.PlaySoundEffect(tg->se_mgsalert);

      if (tg->g_Yes || tg->g_No) { tg->g_aa_interface.Shutdown(); };
      });

    int run_diag = tg->g_aa_interface.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(tg->g_No, false);
    TestGlobals::reset();
  }

  TEST_METHOD(ReuseModelResources) {
    TestGlobals::init();

    // init engine
    {
      bool initSuccess = tg->g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    // camera that stays screen size with a light and fov slider
    {
      tg->g_window_ref = tg->g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();

      tg->g_cam_id = tg->g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      //tg->g_cam_id = tg->g_aa_interface.AddCamera(1, 1);
      tg->g_camera_ref = tg->g_aa_interface.GetCamera(tg->g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = tg->g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(*tg->cam_fov);

      setup_fpp_fly(tg->g_cam_id, tg->g_aa_interface);

      tg->g_aa_interface.SetDirectionalLight(
        glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
        glm::vec3(*tg->dir_light_amb),
        glm::vec3(*tg->dir_light_diff),
        glm::vec3(*tg->dir_light_spec));
    }

    tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("ReuseModelResources");

      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      bool update_dlight_dir = ImGui::SliderFloat3("Sun Direction", tg->dir_light_direction, -1.0f, 1.0f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Sun Ambient", tg->dir_light_amb, 0.003f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Sun Diffuse", tg->dir_light_diff, 0.003f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Sun Spec", tg->dir_light_spec, 0.003f, 1.f, "%f", 1.0f);
      bool update_cam_fov = ImGui::SliderFloat("Cam FOV", tg->cam_fov, 30.f, 90.f);
      tg->g_No = ImGui::Button("report broken");
      tg->g_Yes = ImGui::Button("NEXT TEST");

      ImGui::End();

      // IMGUI INTERFACE STATE UPDATE
      // ----------------------------
      if (doToggleFS) { tg->g_aa_interface.ToggleWindowFullscreen(); };

      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular) {
        tg->g_aa_interface.SetDirectionalLight(
          glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
          glm::vec3(*tg->dir_light_amb),
          glm::vec3(*tg->dir_light_diff),
          glm::vec3(*tg->dir_light_spec));
      }

      if (update_cam_fov) {
        auto cam = tg->g_camera_ref.lock();
        cam->SetFOV(*tg->cam_fov);
      }

      if (tg->g_Yes || tg->g_No) { tg->g_aa_interface.Shutdown(); };

      // --------------------------------
      // END IMGUI INTERFACE STATE UPDATE
      });

    tg->g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });

    // Load and Test Models to make sure thy reuse the same VAO if loading from the same file
    {
      tg->g_untextured_cube_id[0] = tg->g_aa_interface.AddProp(tg->fullcubepath.c_str(), glm::vec3(-20, 0, -25));
      tg->g_untextured_cube_id[1] = tg->g_aa_interface.AddProp(tg->fullcubepath.c_str(), glm::vec3(20, 0, -25));

      auto w1 = tg->g_aa_interface.GetProp(tg->g_untextured_cube_id[0]);
      std::shared_ptr<AA::Prop> s1 = w1.lock();
      auto vao1 = s1->GetMeshes()[0].vao;

      auto w2 = tg->g_aa_interface.GetProp(tg->g_untextured_cube_id[1]);
      std::shared_ptr<AA::Prop> s2 = w2.lock();
      auto vao2 = s2->GetMeshes()[0].vao;

      Assert::AreEqual(vao1, vao2);  // should be from the same vao
    }

    // Load and Test Animated Models to make sure thy reuse the same VAO if loading from the same file
    {
      tg->g_zombie_id[0] = tg->g_aa_interface.AddAnimProp(tg->fullzombie_.c_str(), glm::vec3(-20, -20, -45), glm::vec3(.15f));
      tg->g_punching_anim_id = tg->g_aa_interface.AddAnimation(tg->fullzombie_.c_str(), tg->g_zombie_id[0]);
      tg->g_aa_interface.SetAnimationOnAnimProp(tg->g_punching_anim_id, tg->g_zombie_id[0]);

      tg->g_zombie_id[1] = tg->g_aa_interface.AddAnimProp(tg->fullzombie_.c_str(), glm::vec3(20, -20, -45), glm::vec3(.15f));
      tg->g_punching_anim_id = tg->g_aa_interface.AddAnimation(tg->fullzombie_.c_str(), tg->g_zombie_id[1]);
      tg->g_aa_interface.SetAnimationOnAnimProp(tg->g_punching_anim_id, tg->g_zombie_id[1]);

      auto w1 = tg->g_aa_interface.GetAnimProp(tg->g_zombie_id[0]);
      std::shared_ptr<AA::Prop> s1 = w1.lock();
      auto vao3 = s1->GetMeshes()[0].vao;

      auto w2 = tg->g_aa_interface.GetAnimProp(tg->g_zombie_id[0]);
      std::shared_ptr<AA::Prop> s2 = w2.lock();
      auto vao4 = s2->GetMeshes()[0].vao;

      Assert::AreEqual(vao3, vao4);  // should be from the same vao
    }

    int run_diag = tg->g_aa_interface.Run();

    Assert::AreEqual(run_diag, 0);

    TestGlobals::reset();
  }

  TEST_METHOD(LightingTests) {
    TestGlobals::init();

    // init engine
    {
      AA::WindowOptions win_opts;
      win_opts._windowing_mode = AA::WINDOW_MODE::MAXIMIZED;
      win_opts._title = "LightingTests";
      bool initSuccess = tg->g_aa_interface.Init(win_opts);
      Assert::AreEqual(initSuccess, true);
    }

    // Create a Flying Camera and a Sunlight 
    {
      tg->g_window_ref = tg->g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = tg->g_window_ref.lock();
      tg->g_cam_id = tg->g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      tg->g_camera_ref = tg->g_aa_interface.GetCamera(tg->g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = tg->g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(*tg->cam_fov);
      local_camera_ref->SetPosition(glm::vec3(0, 200, 300));
      local_camera_ref->SetPitch(-10.f);
      setup_fpp_fly(tg->g_cam_id, tg->g_aa_interface);
      tg->g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });

      // a flashlight
      tg->g_slight1_id = tg->g_aa_interface.AddSpotLight(
        local_camera_ref->GetPosition(),
        local_camera_ref->GetFront(),
        *tg->spot_light_inner, // inner
        *tg->spot_light_outer, // outer
        1.0f /* constant*/,
        *tg->spot_light_linear,
        *tg->spot_light_quadratic,
        glm::vec3(*tg->spot_light_ambient),
        glm::vec3(*tg->spot_light_diff),
        glm::vec3(*tg->spot_light_spec)
      );

      // a sunlight
      tg->g_aa_interface.SetDirectionalLight(
        glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
        glm::vec3(*tg->dir_light_amb),
        glm::vec3(*tg->dir_light_diff),
        glm::vec3(*tg->dir_light_spec));

      // ground
      for (int i = -3; i < 4; i++)
        for (int j = -3; j < 4; j++)
          tg->g_aa_interface.AddProp(tg->fullgroundplane.c_str(), glm::vec3(i*400, -30.f, j*400), glm::vec3(1.f));
      
      tg->g_peasant_man_id = tg->g_aa_interface.AddProp(tg->fullpeasant_man.c_str(), glm::vec3(0, -30, -100), glm::vec3(1.f));

      tg->g_plight1_id = tg->g_aa_interface.AddPointLight(
        glm::vec3(tg->point_light_loc[0], tg->point_light_loc[1], tg->point_light_loc[2]),
        1.0f /* constant*/,
        *tg->point_light_linear,
        *tg->point_light_quadratic,
        glm::vec3(*tg->point_light_ambient),
        glm::vec3(*tg->point_light_diff),
        glm::vec3(*tg->point_light_spec));

      tg->g_aa_interface.DebugLightIndicatorsOnOrOff(tg->debug_point_light);
    }

    // ImGui Controls
    tg->g_imgui_func = tg->g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("LightingTests");
      ImGui::Text("SUN");
      bool update_dlight_dir = ImGui::SliderFloat3("Sun Direction", tg->dir_light_direction, -1.0f, 1.0f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Sun Ambient", tg->dir_light_amb, 0.003f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Sun Diffuse", tg->dir_light_diff, 0.003f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Sun Spec", tg->dir_light_spec, 0.003f, 1.f, "%f", 1.0f);

      ImGui::Text("BULB");
      bool update_draw_plight1_loc_cube = ImGui::Checkbox("Draw Debug Cube", &tg->debug_point_light);
      bool update_plight1_loc = ImGui::SliderFloat3("Point Light Location", tg->point_light_loc, -400.f, 400.f, "%f", 1.0f);
      bool update_plight1_linear = ImGui::SliderFloat("Point Light Linear", tg->point_light_linear, 0.0001f, 0.300f, "%f", 1.0f);
      bool update_plight1_quadratic = ImGui::SliderFloat("Point Light Quadratic", tg->point_light_quadratic, 0.0001f, 0.300f, "%f", 1.0f);
      bool update_plight1_ambient = ImGui::SliderFloat("Point Light Ambient", tg->point_light_ambient, 0.f, 1.f, "%f", 1.0f);
      bool update_plight1_diff = ImGui::SliderFloat("Point Light Diffuse", tg->point_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_plight1_spec = ImGui::SliderFloat("Point Light Spec", tg->point_light_spec, 0.f, 1.f, "%f", 1.0f);

      ImGui::Text("FLASHLIGHT");
      bool update_slight1_inner = ImGui::SliderFloat("Spot Light Inner", tg->spot_light_inner, 0.03f, 19.f, "%f", 1.0f);
      bool update_slight1_outer = ImGui::SliderFloat("Spot Light Outer", tg->spot_light_outer, 0.1f, 40.f, "%f", 1.0f);
      bool update_slight1_linear = ImGui::SliderFloat("Spot Light Linear", tg->spot_light_linear, 0.0001f, 0.300f, "%f", 1.0f);
      bool update_slight1_quadratic = ImGui::SliderFloat("Spot Light Quadratic", tg->spot_light_quadratic, 0.0001f, 0.300f, "%f", 1.0f);
      bool update_slight1_ambient = ImGui::SliderFloat("Spot Light Ambient", tg->spot_light_ambient, 0.f, 1.f, "%f", 1.0f);
      bool update_slight1_diff = ImGui::SliderFloat("Spot Light Diffuse", tg->spot_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_slight1_spec = ImGui::SliderFloat("Spot Light Spec", tg->spot_light_spec, 0.f, 1.f, "%f", 1.0f);

      ImGui::Text("VIEWPORT");
      bool update_cam_fov = ImGui::SliderFloat("Cam FOV", tg->cam_fov, 30.f, 90.f);
      bool doToggleFS = ImGui::Button("ToggleFullscreen");

      tg->g_No = ImGui::Button("report broken");
      tg->g_Yes = ImGui::Button("NEXT TEST");
      ImGui::End();

      // state update
      if (doToggleFS) { tg->g_aa_interface.ToggleWindowFullscreen(); };

      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular) {
        tg->g_aa_interface.SetDirectionalLight(
          glm::vec3(tg->dir_light_direction[0], tg->dir_light_direction[1], tg->dir_light_direction[2]),
          glm::vec3(*tg->dir_light_amb),
          glm::vec3(*tg->dir_light_diff),
          glm::vec3(*tg->dir_light_spec));
      }

      if (update_plight1_loc || update_plight1_linear || update_plight1_quadratic || update_plight1_ambient
        || update_plight1_diff || update_plight1_spec) {
        tg->g_aa_interface.ChangePointLight(tg->g_plight1_id, glm::vec3(tg->point_light_loc[0], tg->point_light_loc[1], tg->point_light_loc[2]), *tg->point_light_constant, *tg->point_light_linear, *tg->point_light_quadratic,
          glm::vec3(*tg->point_light_ambient), glm::vec3(*tg->point_light_diff), glm::vec3(*tg->point_light_spec));
      }

      if (/*update_slight1_loc || update_slight1_dir ||*/ update_slight1_inner || update_slight1_outer ||
        update_slight1_linear || update_slight1_quadratic || update_slight1_ambient || update_slight1_diff || update_slight1_spec) {
        auto cam = tg->g_camera_ref.lock();
        auto loc = cam->GetPosition();
        auto dir = cam->GetFront();
        tg->g_aa_interface.MoveSpotLight(tg->g_slight1_id, loc, dir);
        tg->g_aa_interface.ChangeSpotLight(
          tg->g_slight1_id, 
          loc,
          dir, 
          *tg->spot_light_inner, 
          *tg->spot_light_outer, 
          *tg->spot_light_constant, 
          *tg->spot_light_linear, 
          *tg->spot_light_quadratic,
          glm::vec3(*tg->spot_light_ambient), 
          glm::vec3(*tg->spot_light_diff), 
          glm::vec3(*tg->spot_light_spec));
      }

      if (update_draw_plight1_loc_cube) {
        tg->g_aa_interface.DebugLightIndicatorsOnOrOff(tg->debug_point_light);
      }

      // keep spot light on cam location and direction
      {
        auto cam = tg->g_camera_ref.lock();
        auto loc = cam->GetPosition();
        auto dir = cam->GetFront();
        tg->g_aa_interface.MoveSpotLight(tg->g_slight1_id, loc, dir);
      }

      if (update_cam_fov) {
        auto cam = tg->g_camera_ref.lock();
        cam->SetFOV(*tg->cam_fov);
      }

      if (tg->g_Yes || tg->g_No) { tg->g_aa_interface.Shutdown(); };

      });

    int run_diag = tg->g_aa_interface.Run();

    Assert::AreEqual(run_diag, 0);

    Assert::AreEqual(tg->g_No, false);

    TestGlobals::reset();
  }

  // todo: music tests

};
}

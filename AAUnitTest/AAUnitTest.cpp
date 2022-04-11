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
        std::shared_ptr<AA::Camera> night_cam = g_camera_ref.lock();
        night_cam->SetSkybox(NightSkyTextures);
      } else if (Day) {
        std::shared_ptr<AA::Camera> day_cam = g_camera_ref.lock();
        day_cam->SetSkybox(DaySkyTextures);
      } else if (Cave) {
        std::shared_ptr<AA::Camera> cave_cam = g_camera_ref.lock();
        cave_cam->SetSkybox(CaveTextures);
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
      auto t1 = g_aa_interface.GetProp(g_untextured_cube_id[0]);
      std::shared_ptr<AA::Prop> s1 = t1.lock();
      s1->SetRotation(glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
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
      bool update_dlight_dir = ImGui::SliderFloat3("Light Direction", dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Light Ambient", dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Light Diffuse", dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Light Spec", dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      g_Yes = ImGui::Button("Yes");
      g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular)
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

    g_zombie_id[0] = g_aa_interface.AddAnimProp(fullzombie_.c_str(), glm::vec3(0, -30, -75), glm::vec3(.25f));
    g_punching_anim_id = g_aa_interface.AddAnimation(fullzombie_.c_str(), g_zombie_id[0]);
    g_aa_interface.SetAnimationOnAnimProp(g_punching_anim_id, g_zombie_id[0]);

    // default light and background
    g_aa_interface.SetWindowClearColor();

    g_aa_interface.SetDirectionalLight(
      glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
      glm::vec3(*dir_light_amb),
      glm::vec3(*dir_light_diff),
      glm::vec3(*dir_light_spec));

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Animated Model Test");
      bool update_dlight_dir = ImGui::SliderFloat3("Light Direction", dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Light Ambient", dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Light Diffuse", dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Light Spec", dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      g_Yes = ImGui::Button("Yes");
      g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular)
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

  TEST_METHOD(SpaceSceneMashup) {
    // init AA
    {
      bool initSuccess = g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    g_aa_interface.SetWindowTitle("Space Scene Mashup Test");

    // night time skybox files
    {
      const std::string skyboxfolder = runtime_dir + "skybox/night/";
      const std::string order[6] = { "right",  "left",  "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      NightSkyTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        NightSkyTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    // camera that stays screen size
    {
      g_window_ref = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();
      g_cam_id = g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());


      g_camera_ref = g_aa_interface.GetCamera(g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = g_camera_ref.lock();
      local_camera_ref->SetSkybox(NightSkyTextures);
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(*cam_fov);
      setup_fpp_fly(g_cam_id);
    }

    // load models
    //g_walking_man_id = g_aa_interface.AddAnimProp(fullwalking_man.c_str(), glm::vec3(30, -30, -70), glm::vec3(1.f));
    //g_walking_anim_id = g_aa_interface.AddAnimation(fullwalking_man.c_str(), g_walking_man_id);
    //g_aa_interface.SetAnimationOnAnimProp(g_walking_anim_id, g_walking_man_id);

    {
      g_untextured_cube_id[0] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(-10, -10, -10));
      auto w1 = g_aa_interface.GetProp(g_untextured_cube_id[0]);
      std::shared_ptr<AA::Prop> s1 = w1.lock();
      s1->SetRotation(glm::vec3(1.571f, 3.14159f, 0));
      s1->SetStencil(true);
      s1->SetStencilColor(glm::vec3(.4, .4, .4));
      s1->SetStencilWithNormals(false);
      s1->SetStencilScale(1.1f);
    }

    g_ground_plane_id = g_aa_interface.AddProp(fullgroundplane.c_str(), glm::vec3(0, -30.f, 0), glm::vec3(3));


    //g_peasant_man_id = g_aa_interface.AddProp(fullpeasant_man.c_str(), glm::vec3(0, -30, -70), glm::vec3(.15f));



    // Add Zombie With Punching Animation.
    g_zombie_id[0] = g_aa_interface.AddAnimProp(fullzombie_.c_str(), glm::vec3(-30, -30, -70), glm::vec3(0.12f));
    g_punching_anim_id = g_aa_interface.AddAnimation(fullzombie_.c_str(), g_zombie_id[0]);
    g_aa_interface.SetAnimationOnAnimProp(g_punching_anim_id, g_zombie_id[0]);
    {
      auto weak_tmp = g_aa_interface.GetAnimProp(g_zombie_id[0]);
      auto strong_tmp = weak_tmp.lock();
      strong_tmp->SetStencil(true);
      strong_tmp->SetStencilWithNormals(true);
      strong_tmp->SetStencilColor(glm::vec3(0, 9, 0));
      strong_tmp->SetStencilScale(5.f);

    }




    // default light and background
    //g_aa_interface.SetWindowClearColor();

    g_aa_interface.SetDirectionalLight(
      glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
      glm::vec3(*dir_light_amb),
      glm::vec3(*dir_light_diff),
      glm::vec3(*dir_light_spec));

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Space Scene Mashup");
      ImGui::Text("Directional Light Controls");
      bool update_dlight_dir = ImGui::SliderFloat3("Direction", dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Diffuse", dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Ambient", dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Specular", dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Camera Controls");
      bool update_cam_fov = ImGui::SliderFloat("FOV", cam_fov, 30.f, 90.f);
      g_No = ImGui::Button("report broken");
      g_Yes = ImGui::Button("QUIT");
      ImGui::End();

      // state update
      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular)
        g_aa_interface.SetDirectionalLight(
          glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
          glm::vec3(*dir_light_amb),
          glm::vec3(*dir_light_diff),
          glm::vec3(*dir_light_spec));
      if (update_cam_fov) {
        auto cam = g_camera_ref.lock();
        cam->SetFOV(*cam_fov);
      }
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
    {
      g_untextured_cube_id[0] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(0, 0, -10));
      auto w1 = g_aa_interface.GetProp(g_untextured_cube_id[0]);
      std::shared_ptr<AA::Prop> s1 = w1.lock();
      s1->SetStencil(true);
      s1->SetStencilColor(glm::vec3(.4, .4, .4));
      s1->SetStencilWithNormals(false);
      s1->SetStencilScale(1.1f);

      g_untextured_cube_id[1] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(-10, 0, -10));
      g_untextured_cube_id[2] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(10, 0, -10));

      g_ground_plane_id = g_aa_interface.AddProp(fullgroundplane.c_str(), glm::vec3(0, -30.f, 0), glm::vec3(3, 1, 3));

      g_peasant_man_id = g_aa_interface.AddProp(fullpeasant_man.c_str(), glm::vec3(0, -30, -100), glm::vec3(.25f));
      auto w2 = g_aa_interface.GetProp(g_peasant_man_id);
      std::shared_ptr<AA::Prop> s2 = w2.lock();
      s2->SetStencil(true);
      s2->SetStencilColor(glm::vec3(.4, .4, .4));
      s2->SetStencilWithNormals(true);
      s2->SetStencilScale(3.f);

      g_walking_man_id = g_aa_interface.AddProp(fullwalking_man.c_str(), glm::vec3(-60, -30, -100), glm::vec3(.25f));
      auto w3 = g_aa_interface.GetProp(g_walking_man_id);
      std::shared_ptr<AA::Prop> s3 = w3.lock();
      s3->SetStencil(true);
      s3->SetStencilColor(glm::vec3(.4, .4, .4));
      s3->SetStencilWithNormals(true);
      s3->SetStencilScale(3.f);
    }

    g_aa_interface.AddToUpdate([](float dt) {
      static float accum_time = 0;
      accum_time += dt;
      auto t1 = g_aa_interface.GetProp(g_untextured_cube_id[0]);
      auto t2 = g_aa_interface.GetProp(g_untextured_cube_id[1]);
      auto t3 = g_aa_interface.GetProp(g_untextured_cube_id[2]);
      std::shared_ptr<AA::Prop> st1 = t1.lock();
      std::shared_ptr<AA::Prop> st2 = t2.lock();
      std::shared_ptr<AA::Prop> st3 = t3.lock();
      st1->SetRotation(glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      st2->SetRotation(glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      st3->SetRotation(glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      });

    // default light and background
    g_aa_interface.SetWindowClearColor();
    g_aa_interface.SetDirectionalLight(
      glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
      glm::vec3(*dir_light_amb),
      glm::vec3(*dir_light_diff),
      glm::vec3(*dir_light_spec));

    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("Stencil Outline Test");
      bool update_dlight_dir = ImGui::SliderFloat3("Light Direction", dir_light_direction, -1.f, 1.f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Light Ambient", dir_light_amb, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Light Diffuse", dir_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Light Spec", dir_light_spec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
      g_Yes = ImGui::Button("Yes");
      g_No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular)
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

  TEST_METHOD(ReuseModelResources) {
    // init engine
    {
      bool initSuccess = g_aa_interface.Init();
      Assert::AreEqual(initSuccess, true);
    }

    // camera that stays screen size with a light and fov slider
    {
      g_window_ref = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();

      g_cam_id = g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      //g_cam_id = g_aa_interface.AddCamera(1, 1);
      g_camera_ref = g_aa_interface.GetCamera(g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(*cam_fov);

      setup_fpp_fly(g_cam_id);

      g_aa_interface.SetDirectionalLight(
        glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
        glm::vec3(*dir_light_amb),
        glm::vec3(*dir_light_diff),
        glm::vec3(*dir_light_spec));
    }

    g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("ReuseModelResources");

      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      bool update_dlight_dir = ImGui::SliderFloat3("Sun Direction", dir_light_direction, -1.0f, 1.0f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Sun Ambient", dir_light_amb, 0.003f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Sun Diffuse", dir_light_diff, 0.003f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Sun Spec", dir_light_spec, 0.003f, 1.f, "%f", 1.0f);
      bool update_cam_fov = ImGui::SliderFloat("Cam FOV", cam_fov, 30.f, 90.f);
      g_No = ImGui::Button("report broken");
      g_Yes = ImGui::Button("NEXT TEST");

      ImGui::End();

      // IMGUI INTERFACE STATE UPDATE
      // ----------------------------
      if (doToggleFS) { g_aa_interface.ToggleWindowFullscreen(); };

      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular) {
        g_aa_interface.SetDirectionalLight(
          glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
          glm::vec3(*dir_light_amb),
          glm::vec3(*dir_light_diff),
          glm::vec3(*dir_light_spec));
      }

      if (update_cam_fov) {
        auto cam = g_camera_ref.lock();
        cam->SetFOV(*cam_fov);
      }

      if (g_Yes || g_No) { g_aa_interface.Shutdown(); };

      // --------------------------------
      // END IMGUI INTERFACE STATE UPDATE
      });

    g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });


    // Load and Test Models to make sure thy reuse the same VAO if loading from the same file
    {
      g_untextured_cube_id[0] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(-20, 0, -25));
      g_untextured_cube_id[1] = g_aa_interface.AddProp(fullcubepath.c_str(), glm::vec3(20, 0, -25));

      auto w1 = g_aa_interface.GetProp(g_untextured_cube_id[0]);
      std::shared_ptr<AA::Prop> s1 = w1.lock();
      auto vao1 = s1->GetMeshes()[0].vao;

      auto w2 = g_aa_interface.GetProp(g_untextured_cube_id[1]);
      std::shared_ptr<AA::Prop> s2 = w2.lock();
      auto vao2 = s2->GetMeshes()[0].vao;

      Assert::AreEqual(vao1, vao2);  // should be from the same vao
    }

    // Load and Test Animated Models to make sure thy reuse the same VAO if loading from the same file
    {
      g_zombie_id[0] = g_aa_interface.AddAnimProp(fullzombie_.c_str(), glm::vec3(-20, -20, -45), glm::vec3(.15f));
      g_punching_anim_id = g_aa_interface.AddAnimation(fullzombie_.c_str(), g_zombie_id[0]);
      g_aa_interface.SetAnimationOnAnimProp(g_punching_anim_id, g_zombie_id[0]);

      g_zombie_id[1] = g_aa_interface.AddAnimProp(fullzombie_.c_str(), glm::vec3(20, -20, -45), glm::vec3(.15f));
      g_punching_anim_id = g_aa_interface.AddAnimation(fullzombie_.c_str(), g_zombie_id[1]);
      g_aa_interface.SetAnimationOnAnimProp(g_punching_anim_id, g_zombie_id[1]);

      auto w1 = g_aa_interface.GetAnimProp(g_zombie_id[0]);
      std::shared_ptr<AA::Prop> s1 = w1.lock();
      auto vao3 = s1->GetMeshes()[0].vao;

      auto w2 = g_aa_interface.GetAnimProp(g_zombie_id[0]);
      std::shared_ptr<AA::Prop> s2 = w2.lock();
      auto vao4 = s2->GetMeshes()[0].vao;

      Assert::AreEqual(vao3, vao4);  // should be from the same vao
    }

    int run_diag = g_aa_interface.Run();

    Assert::AreEqual(run_diag, 0);

    reset_test_globals();
  }

  TEST_METHOD(LightingTests) {
    // init engine
    {
      AA::WindowOptions win_opts;
      win_opts._windowing_mode = AA::WINDOW_MODE::MAXIMIZED;
      win_opts._title = "LightingTests";
      bool initSuccess = g_aa_interface.Init(win_opts);
      Assert::AreEqual(initSuccess, true);
    }

    // Create a Flying Camera and a Sunlight 
    {
      g_window_ref = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_window_ref = g_window_ref.lock();
      g_cam_id = g_aa_interface.AddCamera(local_window_ref->GetCurrentWidth(), local_window_ref->GetCurrentHeight());
      g_camera_ref = g_aa_interface.GetCamera(g_cam_id);
      std::shared_ptr<AA::Camera> local_camera_ref = g_camera_ref.lock();
      local_camera_ref->SetKeepCameraToWindowSize(true);
      local_camera_ref->SetFOV(*cam_fov);
      local_camera_ref->SetPosition(glm::vec3(0, 200, 300));
      local_camera_ref->SetPitch(-10.f);
      setup_fpp_fly(g_cam_id);
      g_aa_interface.AddToOnQuit([]() { turn_off_fly(); });

      // a light so we can see
      g_aa_interface.SetDirectionalLight(
        glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
        glm::vec3(*dir_light_amb),
        glm::vec3(*dir_light_diff),
        glm::vec3(*dir_light_spec));
    
      // ground and a house
      g_ground_plane_id = g_aa_interface.AddProp(fullgroundplane.c_str(), glm::vec3(0, -30.f, 0), glm::vec3(20));

    
      g_peasant_man_id = g_aa_interface.AddProp(fullpeasant_man.c_str(), glm::vec3(0, -30, -100), glm::vec3(1.f));
    }

    g_aa_interface.DebugLightIndicatorsOnOrOff(false);

    g_plight1_id = g_aa_interface.AddPointLight(
      glm::vec3(point_light_loc[0], point_light_loc[1], point_light_loc[2]),
      1.0f /* constant*/,
      *point_light_linear,
      *point_light_quadratic,
      glm::vec3(*point_light_ambient),
      glm::vec3(*point_light_diff),
      glm::vec3(*point_light_spec));

    /* sig: glm::vec3 pos, glm::vec3 dir, float inner, float outer, float constant, float linear, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec */
    g_slight1_id = g_aa_interface.AddSpotLight(
      glm::vec3(spot_light_loc[0], spot_light_loc[1], spot_light_loc[2]),
      glm::vec3(spot_light_dir[0], spot_light_dir[1], spot_light_dir[2]),
      *spot_light_inner, // inner
      *spot_light_outer, // outer
      1.0f /* constant*/,
      *spot_light_linear,
      *spot_light_quadratic,
      glm::vec3(*spot_light_ambient),
      glm::vec3(*spot_light_diff),
      glm::vec3(*spot_light_spec)
    );

    // ImGui Controls
    g_imgui_func = g_aa_interface.AddToImGuiUpdate([]() {
      ImGui::Begin("LightingTests");
      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      ImGui::Text("SUN");
      bool update_dlight_dir = ImGui::SliderFloat3("Sun Direction", dir_light_direction, -1.0f, 1.0f, "%f", 1.0f);
      bool update_dlight_amb = ImGui::SliderFloat("Sun Ambient", dir_light_amb, 0.003f, 1.f, "%f", 1.0f);
      bool update_dlight_diffuse = ImGui::SliderFloat("Sun Diffuse", dir_light_diff, 0.003f, 1.f, "%f", 1.0f);
      bool update_dlight_specular = ImGui::SliderFloat("Sun Spec", dir_light_spec, 0.003f, 1.f, "%f", 1.0f);

      ImGui::Text("BULB");
      bool update_plight1_loc = ImGui::SliderFloat3("Point Light Location", point_light_loc, -400.f, 400.f, "%f", 1.0f);
      bool update_plight1_linear = ImGui::SliderFloat("PL Linear", point_light_linear, 0.0001f, 0.300f, "%f", 1.0f);
      bool update_plight1_quadratic = ImGui::SliderFloat("PL Quadratic", point_light_quadratic, 0.0001f, 0.300f, "%f", 1.0f);
      bool update_plight1_ambient = ImGui::SliderFloat("PL Ambient", point_light_ambient, 0.f, 1.f, "%f", 1.0f);
      bool update_plight1_diff = ImGui::SliderFloat("PL Diffuse", point_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_plight1_spec = ImGui::SliderFloat("PL Spec", point_light_spec, 0.f, 1.f, "%f", 1.0f);

      ImGui::Text("FLASHLIGHT");
      //bool update_slight1_loc = ImGui::SliderFloat3("Spot Light Location", spot_light_loc, -400.f, 400.f, "%f", 1.0f);
      //bool update_slight1_dir = ImGui::SliderFloat3("Spot Light Direction", spot_light_dir, -1.f, 1.f, "%f", 1.0f);
      bool update_slight1_inner = ImGui::SliderFloat("SL Inner", spot_light_inner, 0.03f, 19.f, "%f", 1.0f);
      bool update_slight1_outer = ImGui::SliderFloat("SL Outer", spot_light_outer, 0.1f, 40.f, "%f", 1.0f);
      bool update_slight1_linear = ImGui::SliderFloat("SL Linear", spot_light_linear, 0.0001f, 0.300f, "%f", 1.0f);
      bool update_slight1_quadratic = ImGui::SliderFloat("SL Quadratic", spot_light_quadratic, 0.0001f, 0.300f, "%f", 1.0f);
      bool update_slight1_ambient = ImGui::SliderFloat("SL Ambient", spot_light_ambient, 0.f, 1.f, "%f", 1.0f);
      bool update_slight1_diff = ImGui::SliderFloat("SL Diffuse", spot_light_diff, 0.f, 1.f, "%f", 1.0f);
      bool update_slight1_spec = ImGui::SliderFloat("SL Spec", spot_light_spec, 0.f, 1.f, "%f", 1.0f);

      ImGui::Text("VIEWPORT");
      bool update_cam_fov = ImGui::SliderFloat("Cam FOV", cam_fov, 30.f, 90.f);

      g_No = ImGui::Button("report broken");
      g_Yes = ImGui::Button("NEXT TEST");
      ImGui::End();

      // state update
      if (doToggleFS) { g_aa_interface.ToggleWindowFullscreen(); };

      if (update_dlight_dir || update_dlight_amb || update_dlight_diffuse || update_dlight_specular) {
        g_aa_interface.SetDirectionalLight(
          glm::vec3(dir_light_direction[0], dir_light_direction[1], dir_light_direction[2]),
          glm::vec3(*dir_light_amb),
          glm::vec3(*dir_light_diff),
          glm::vec3(*dir_light_spec));
      }

      if (update_plight1_loc || update_plight1_linear || update_plight1_quadratic || update_plight1_ambient
        || update_plight1_diff || update_plight1_spec) {
        g_aa_interface.ChangePointLight(g_plight1_id, glm::vec3(point_light_loc[0], point_light_loc[1], point_light_loc[2]), *point_light_constant, *point_light_linear, *point_light_quadratic,
          glm::vec3(*point_light_ambient), glm::vec3(*point_light_diff), glm::vec3(*point_light_spec));
      }

      if (/*update_slight1_loc || update_slight1_dir ||*/ update_slight1_inner || update_slight1_outer ||
        update_slight1_linear || update_slight1_quadratic || update_slight1_ambient || update_slight1_diff || update_slight1_spec) {
        g_aa_interface.ChangeSpotLight(
          g_slight1_id, 
          glm::vec3(spot_light_loc[0], spot_light_loc[1], spot_light_loc[2]), 
          glm::vec3(spot_light_dir[0] + spot_light_loc[0], spot_light_dir[1] + spot_light_loc[1], spot_light_dir[2] + spot_light_loc[2]), 
          *spot_light_inner, 
          *spot_light_outer, 
          *spot_light_constant, 
          *spot_light_linear, 
          *spot_light_quadratic,
          glm::vec3(*spot_light_ambient), 
          glm::vec3(*spot_light_diff), 
          glm::vec3(*spot_light_spec));
      }


      // keep spot light on cam location and direction
      {
        auto cam = g_camera_ref.lock();
        auto loc = cam->GetPosition();
        auto dir = cam->GetFront();
        g_aa_interface.MoveSpotLight(g_slight1_id, loc, dir);
      }

      if (update_cam_fov) {
        auto cam = g_camera_ref.lock();
        cam->SetFOV(*cam_fov);
      }

      if (g_Yes || g_No) { g_aa_interface.Shutdown(); };

      });

    int run_diag = g_aa_interface.Run();

    Assert::AreEqual(run_diag, 0);

    Assert::AreEqual(g_No, false);

    reset_test_globals();
  }

  // todo: music tests

};
}

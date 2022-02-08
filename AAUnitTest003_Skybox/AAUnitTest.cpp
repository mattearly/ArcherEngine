#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
extern AA::Interface instance;
namespace AAUnitTest
{
TEST_CLASS(AAUnitTest)
{
public:
  TEST_METHOD(MainTestMethod) {

    // load skymap files
    static std::vector<std::string> NightSkyTextures;
    static std::vector<std::string> DaySkyTextures;

    //https://stackoverflow.com/questions/25150708/how-to-get-the-projectdir-path-in-a-visual-studio-native-c-unit-testing-usi
    std::string s = EXPAND(UNITTESTPRJ);
    s.erase(0, 1); // erase the first quote
    s.erase(s.size() - 2); // erase the last quote and the dot
    std::string my_project_dir = s;

    {
      // night time skybox files
      const std::string skyboxfolder = s + "res/skybox/night/";
      const std::string order[6] = { "right", "left", "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      NightSkyTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        NightSkyTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    {
      // day time skybox files
      const std::string skyboxfolder = s + "res/skybox/day/elyvisions/";
      const std::string order[6] = { "right", "left", "top", "bottom", "front", "back" };
      const std::string skyboxfileext = ".png";
      DaySkyTextures.reserve(6);
      for (int j = 0; j < 6; ++j)
        DaySkyTextures.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    }

    // init engine
    bool initSuccess = instance.Init();
    Assert::AreEqual(initSuccess, true);

    // add a camera that fills full screens
    static auto cam_id = instance.AddCamera(instance.GetWindowWidth(), instance.GetWindowHeight());
    auto cam = instance.GetCamera(cam_id);
    cam->SetKeepCameraToWindowSize(true);
    instance.AddToUpdate([](float dt) {
      static float accum_time = 0.f;
      accum_time += dt;
      auto cam = instance.GetCamera(cam_id);
      cam->ShiftPitchAndYaw(0, dt * 2);
      cam->SetPitch(sin(accum_time)*2);
      });


    static bool UserSaidSkyboxFailed = false;

    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("SkyboxTest");
      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      if (doToggleFS) { instance.ToggleWindowFullscreen(); };
      bool Day = ImGui::Button("Switch To Day Skybox");
      bool Night = ImGui::Button("Switch To Night Skybox");
      ImGui::Spacing();
      ImGui::Text("Do Both Skyboxes Work Okay?");
      bool Yes = ImGui::Button("Yes");
      UserSaidSkyboxFailed = ImGui::Button("No");
      ImGui::End();
      if (Night) {
        instance.SetSkybox(NightSkyTextures);
      }
      if (Day) {
        instance.SetSkybox(DaySkyTextures);
      }
      if (Yes || UserSaidSkyboxFailed) { 
        instance.Shutdown(); 
      };
      });


    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);

    Assert::AreEqual(UserSaidSkyboxFailed, false);
  }
};
}

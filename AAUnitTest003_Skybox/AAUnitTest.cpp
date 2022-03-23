#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
extern AA::Interface instance;
namespace AAUnitTest {
TEST_CLASS(AAUnitTest) {
public: TEST_METHOD(MainTestMethod) {

  // resources
  // load skymap files
  static std::vector<std::string> NightSkyTextures;
  static std::vector<std::string> DaySkyTextures;
  static std::vector<std::string> CaveTextures;

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
  bool initSuccess = instance.Init();
  Assert::AreEqual(initSuccess, true);

  // add a camera that fills full screens
  auto win = instance.GetWindow();
  static auto cam_id = instance.AddCamera(win->GetCurrentWidth(), win->GetCurrentHeight());
  auto cam = instance.GetCamera(cam_id);
  cam->SetKeepCameraToWindowSize(true);
  instance.AddToUpdate([](float dt) {
    static float accum_time = 0.f;
    accum_time += dt;
    auto cam = instance.GetCamera(cam_id);
    cam->ShiftPitchAndYaw(0, dt * 2);
    cam->SetPitch(sin(accum_time) * 2);
    });

  static bool UserSaidSkyboxFailed = false;

  instance.AddToImGuiUpdate([]() {
    ImGui::Begin("SkyboxTest");
    bool doToggleFS = ImGui::Button("ToggleFullscreen");
    if (doToggleFS) {
      instance.ToggleWindowFullscreen();
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
      instance.SetSkybox(NightSkyTextures);
    } else if (Day) {
      instance.SetSkybox(DaySkyTextures);
    } else if (Cave) {
      instance.SetSkybox(CaveTextures);
    } else if (Done || UserSaidSkyboxFailed) {
      instance.Shutdown();
    };
    });

  int run_diag = instance.Run();
  Assert::AreEqual(run_diag, 0);

  Assert::AreEqual(UserSaidSkyboxFailed, false);
}  // namespace AAUnitTest
};
}

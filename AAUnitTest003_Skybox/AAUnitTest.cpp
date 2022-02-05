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
    bool initSuccess = instance.Init();
    Assert::AreEqual(initSuccess, true);
    auto cam_id = instance.AddCamera(instance.GetWindowWidth(), instance.GetWindowHeight());
    auto cam = instance.GetCamera(cam_id);
    cam->SetKeepCameraToWindowSize(true);
    
    //https://stackoverflow.com/questions/25150708/how-to-get-the-projectdir-path-in-a-visual-studio-native-c-unit-testing-usi
    std::string s = EXPAND(UNITTESTPRJ);
    s.erase(0, 1); // erase the first quote
    s.erase(s.size() - 2); // erase the last quote and the dot
    std::string my_project_dir = s;

    const std::string skyboxfolder = s + "res/skybox/night/";
    const std::string order[6] = { "right", "left", "top", "bottom", "front", "back" };
    const std::string skyboxfileext = ".png";
    std::vector<std::string> cubemapfiles;
    cubemapfiles.reserve(6);
    for (int j = 0; j < 6; ++j)
      cubemapfiles.emplace_back(skyboxfolder + order[j] + skyboxfileext);
    instance.SetSkybox(cubemapfiles);
    static bool No = false;
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Run Test");
      ImGui::Text("Do you See a Skybox?");
      bool Yes = ImGui::Button("Yes");
      No = ImGui::Button("No");
      if (Yes || No) { instance.Shutdown(); };
      ImGui::End();
      });
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Options Test");
      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      if (doToggleFS) { instance.ToggleWindowFullscreen(); };
      ImGui::End();
      });
    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(No, false);
  }
};
}

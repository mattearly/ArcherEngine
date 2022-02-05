#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
extern AA::Interface instance;
namespace AAUnitTest
{
TEST_CLASS(AAUnitTest)
{
public:

  TEST_METHOD(MainTestMethod)
  {
    bool initSuccess = instance.Init();
    Assert::AreEqual(initSuccess, true);
    instance.SetWindowClearColor();

    auto cam_id = instance.AddCamera(instance.GetWindowWidth(), instance.GetWindowHeight());
    auto cam = instance.GetCamera(cam_id);
    cam->SetKeepCameraToWindowSize(true);

    //https://stackoverflow.com/questions/25150708/how-to-get-the-projectdir-path-in-a-visual-studio-native-c-unit-testing-usi
    std::string s = EXPAND(UNITTESTPRJ);
    s.erase(0, 1); // erase the first quote
    s.erase(s.size() - 2); // erase the last quote and the dot
    std::string my_project_dir = s;
    const char* cubepath = "res/3dmodels/cube.glb";
    std::string fullpath = s + cubepath;
    instance.AddProp(fullpath.c_str(), glm::vec3(0, 0, -60));

    static bool No = false;
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Run Test");
      ImGui::Text("Do you see the cube?");
      bool Yes = ImGui::Button("Yes");
      No = ImGui::Button("No");
      if (Yes || No) { instance.Shutdown(); };
      ImGui::End();
      });
    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(No, false);
  }
};
}

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
    AA::WindowOptions winopts;
    winopts._vsync = true;
    bool initSuccess = instance.Init(winopts);
    Assert::AreEqual(initSuccess, true);
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Options Test");
      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      if (doToggleFS) { instance.ToggleWindowFullscreen(); };
      ImGui::End();
      });
    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
  }
};
}

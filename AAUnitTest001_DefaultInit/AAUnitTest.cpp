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
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("default init");
      ImGui::Text("Click \"ok\" to continue");
      bool result = ImGui::Button("ok");
      if (result) { instance.Shutdown(); };
      ImGui::End();
      });
    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
  }
};
}

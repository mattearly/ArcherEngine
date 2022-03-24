// test init, de-init, re-init and soft reset sequences

#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
extern AA::Interface instance;

namespace AAUnitTest
{
TEST_CLASS(AAUnitTest) {
public:

  TEST_METHOD(MainTestMethod) {
    bool initSuccess = instance.Init();
    Assert::AreEqual(initSuccess, true);
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Run Test 1");
      ImGui::Text("Click \"ok\" to continue");
      bool result = ImGui::Button("ok");
      ImGui::End();

      // update state
      if (result) { instance.Shutdown(); };
      });
    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
  }

  TEST_METHOD(SecondTestMethod) {
    bool initSuccess = instance.Init();
    Assert::AreEqual(initSuccess, true);
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Run Test 2");
      ImGui::Text("Click \"ok\" to continue");
      bool result = ImGui::Button("ok");
      ImGui::End();

      // update state
      if (result) { instance.Shutdown(); };
      });
    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
  }
};
}

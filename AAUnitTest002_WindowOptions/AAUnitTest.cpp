#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
extern AA::Interface instance;
namespace AAUnitTest
{
TEST_CLASS(AAUnitTest) {
public:

  TEST_METHOD(MainTestMethod) {
    {
      AA::WindowOptions winopts;
      winopts._vsync = true;
      winopts._title = "WindowOptions";
      winopts._cursor_mode = AA::CURSOR_MODE::NORMAL;
      winopts._height = -1;  // should enfore min size instead of -1
      winopts._width = -1;
      winopts._min_width = 500;  // should enfore this instead of engine const _MIN_SIZE
      winopts._min_height = 500;
      bool initSuccess = instance.Init(winopts);
      Assert::AreEqual(initSuccess, true);
    }

    static bool No = false;
    static int min_w_h[2] = { 0,0 };

    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Options Test");
      bool doToggleFS = ImGui::Button("ToggleFullscreen");
      bool WinSizeIncr = ImGui::Button("+Width/Height");
      bool WinSizeDecr = ImGui::Button("-Width/Height");
      bool MinSizeChanged = ImGui::InputInt2("MinWindowWidthHeight", min_w_h);
      ImGui::Text("Does Everything Work As Expected?");
      bool Yes = ImGui::Button("Yes");
      No = ImGui::Button("No");
      ImGui::End();

      // update state
      if (WinSizeIncr) {
        auto win = instance.GetWindow();
        int new_width = win->GetCurrentWidth() + 50;
        int new_height = win->GetCurrentHeight() + 50;
        win->SetNewWidthAndHeight(new_width, new_height);
        Assert::AreEqual(win->GetCurrentWidth(), new_width);
        Assert::AreEqual(win->GetCurrentHeight(), new_height);
      }
      if (doToggleFS) { instance.ToggleWindowFullscreen(); };
      if (!MinSizeChanged) {
        auto win = instance.GetWindow();
        win->SetNewMinWidthAndHeight(min_w_h[0], min_w_h[1]);
        min_w_h[0] = win->GetCurrentMinWidth();
        min_w_h[1] = win->GetCurrentMinHeight();
      }
      if (WinSizeDecr) {
        auto win = instance.GetWindow();
        int new_width = win->GetCurrentWidth() - 50;
        int new_height = win->GetCurrentHeight() - 50;
        win->SetNewWidthAndHeight(new_width, new_height);
      }
      if (Yes || No) { instance.Shutdown(); };
      });

    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(No, false);

  }
};
}

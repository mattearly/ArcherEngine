#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
extern AA::Interface instance;
namespace AAUnitTest
{
TEST_CLASS(AAUnitTest) {
public:

  TEST_METHOD(MainTestMethod) {
    static unsigned int se_coins1(0);
    static unsigned int se_flashlight(0);
    static unsigned int se_gunshot(0);
    static unsigned int se_mgsalert(0);
    static bool No = false;
    {
      // asset paths
      std::string path_se_coins1 = runtime_dir + "soundeffects/coins.ogg";
      std::string path_se_flashlightclick = runtime_dir + "soundeffects/flashlightclick.wav";
      std::string path_se_gunshot = runtime_dir + "soundeffects/GunShotSingle.wav";
      std::string path_se_mgsalert = runtime_dir + "soundeffects/Metal Gear Solid Alert.ogg";

      bool initSuccess = instance.Init();
      Assert::AreEqual(initSuccess, true);

      se_coins1 = instance.AddSoundEffect(path_se_coins1.c_str());
      se_flashlight = instance.AddSoundEffect(path_se_flashlightclick.c_str());
      se_gunshot = instance.AddSoundEffect(path_se_gunshot.c_str());
      se_mgsalert = instance.AddSoundEffect(path_se_mgsalert.c_str());
    }

    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Sound Test");
      bool s1 = ImGui::Button("Play Coin Sound 1 (.ogg)");
      bool s2 = ImGui::Button("Play Flashlight Sound (.wav)");
      bool s3 = ImGui::Button("Play Gunshot Sound (.wav)");
      bool s4 = ImGui::Button("Play MGS Alert Sound (.ogg)");

      ImGui::Text("Do all sound effects work?");
      bool yes = ImGui::Button("Yes");
      No = ImGui::Button("No");
      ImGui::End();

      // update state
      if (s1) instance.PlaySoundEffect(se_coins1);
      if (s2) instance.PlaySoundEffect(se_flashlight);
      if (s3) instance.PlaySoundEffect(se_gunshot);
      if (s4) instance.PlaySoundEffect(se_mgsalert);

      if (yes || No) { instance.Shutdown(); };
      });

    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(No, false);
  }
};
}

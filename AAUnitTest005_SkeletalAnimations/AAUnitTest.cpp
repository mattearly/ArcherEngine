#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
extern AA::Interface instance;
extern void setup_fpp_fly(unsigned int);
namespace AAUnitTest
{
	TEST_CLASS(AAUnitTest)
	{
	public:
		
		TEST_METHOD(MainTestMethod)
		{
      const char* zombie_ = "res/3dmodels/Zombie Punching.fbx";
      std::string fullzombie_ = getProjDir() + zombie_;
      //const char* Walking_man = "res/3dmodels/Walking.dae";
      //std::string fullWalking_man = getProjDir() + Walking_man;

      bool initSuccess = instance.Init();
      Assert::AreEqual(initSuccess, true);

      auto win = instance.GetWindow();
      auto cam_id = instance.AddCamera(win->GetCurrentWidth(), win->GetCurrentHeight());
      auto cam = instance.GetCamera(cam_id);
      cam->SetKeepCameraToWindowSize(true);
      setup_fpp_fly(cam_id);

      auto animpropid1 = instance.AddAnimProp(fullzombie_.c_str(), glm::vec3(0,-40,-400));
      auto punchinganim = instance.AddAnimation(fullzombie_.c_str(), animpropid1);
      instance.SetAnimationOnAnimProp(punchinganim, animpropid1);

      // default light and background
      instance.SetWindowClearColor();
      
      static float lightdir[3] = { 1.f, .5f,-1.f };
      static float* lightamb = new float(.1f);
      static float* lightdiff = new float(1.f);
      static float* lightspec = new float(.5f);
      
      instance.SetDirectionalLight(
        glm::vec3(lightdir[0], lightdir[1], lightdir[2]),
        glm::vec3(*lightamb),
        glm::vec3(*lightdiff),
        glm::vec3(*lightspec));

      instance.AddToImGuiUpdate([]() {
        ImGui::Begin("Run Test");
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

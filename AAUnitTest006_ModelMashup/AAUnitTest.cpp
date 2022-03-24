#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
extern AA::Interface instance;
extern void setup_fpp_fly(unsigned int);
namespace AAUnitTest
{
TEST_CLASS(AAUnitTest) {
public:
  TEST_METHOD(MainTestMethod) {
    // resources
    const char* cubepath = "3dmodels/cube.glb";
    std::string fullcubepath = runtime_dir + cubepath;
    const char* groundplane = "3dmodels/large_green_plane.obj";
    std::string fullgroundplane = runtime_dir + groundplane;
    const char* peasant_man = "3dmodels/peasant_man.dae";
    std::string fullpeasant_man = runtime_dir + peasant_man;

    const char* zombie_ = "3dmodels/Zombie Punching.fbx";
    std::string fullzombie_ = runtime_dir + zombie_;
    
    const char* walking_man_path = "3dmodels/Walking.dae";
    std::string walking_man = runtime_dir + walking_man_path;

    // init AA
    bool initSuccess = instance.Init();
    Assert::AreEqual(initSuccess, true);

    // camera
    auto win = instance.GetWindow();
    auto cam_id = instance.AddCamera(win->GetCurrentWidth(), win->GetCurrentHeight());
    auto cam = instance.GetCamera(cam_id);
    cam->SetKeepCameraToWindowSize(true);
    cam->SetFOV(75.f);
    setup_fpp_fly(cam_id);

    // load models
    static int cube_id = instance.AddProp(fullcubepath.c_str(), glm::vec3(-10, -10, -10));
    static int groundplane_id = instance.AddProp(fullgroundplane.c_str(), glm::vec3(0, -30.f, 0));
    instance.ScaleProp(groundplane_id, glm::vec3(2));
    static int peasant_man_id = instance.AddProp(fullpeasant_man.c_str(), glm::vec3(0, -10, -50), glm::vec3(.5f));
    instance.ScaleProp(peasant_man_id, glm::vec3(.25f));

    auto animpropid1 = instance.AddAnimProp(fullzombie_.c_str(), /*pos*/glm::vec3(20, -10, -50), /*scale*/glm::vec3(.35f));
    auto punchinganim = instance.AddAnimation(fullzombie_.c_str(), animpropid1);
    instance.SetAnimationOnAnimProp(punchinganim, animpropid1);

    auto animpropid2 = instance.AddAnimProp(walking_man.c_str(), /*pos*/glm::vec3(10, -10, 50), /*scale*/glm::vec3(5.f));
    auto walkinganim = instance.AddAnimation(walking_man.c_str(), animpropid2);
    instance.SetAnimationOnAnimProp(walkinganim, animpropid2);

    // default light and background
    instance.SetWindowClearColor();

    static float lightdir[3] = { -0.095f, .71f, -.2f };
    static float* lightamb = new float(.35f);
    static float* lightdiff = new float(.58f);
    static float* lightspec = new float(.47f);

    instance.SetDirectionalLight(
      glm::vec3(lightdir[0], lightdir[1], lightdir[2]),
      glm::vec3(*lightamb),
      glm::vec3(*lightdiff),
      glm::vec3(*lightspec));

    static bool No = false;
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Animated Model Test");
      bool update_light1 = ImGui::SliderFloat3("Light Direction", lightdir, -1.f, 1.f, "%f", 1.0f);
      bool update_light2 = ImGui::SliderFloat("Light Ambient", lightamb, 0.f, 1.f, "%f", 1.0f);
      bool update_light3 = ImGui::SliderFloat("Light Diffuse", lightdiff, 0.f, 1.f, "%f", 1.0f);
      bool update_light4 = ImGui::SliderFloat("Light Spec", lightspec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Do this look okay?");
      bool Yes = ImGui::Button("Yes");
      No = ImGui::Button("No");
      ImGui::End();

      // state update
      if (update_light1 || update_light2 || update_light3 || update_light4)
        instance.SetDirectionalLight(
          glm::vec3(lightdir[0], lightdir[1], lightdir[2]),
          glm::vec3(*lightamb),
          glm::vec3(*lightdiff),
          glm::vec3(*lightspec));
      if (Yes || No) { instance.Shutdown(); };
      });

    int run_diag = instance.Run();
    Assert::AreEqual(run_diag, 0);
    Assert::AreEqual(No, false);
  }
};
}

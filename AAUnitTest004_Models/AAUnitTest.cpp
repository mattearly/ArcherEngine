#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
extern AA::Interface instance;
extern void setup_fpp_fly(unsigned int);
namespace AAUnitTest {
TEST_CLASS(AAUnitTest) {
public:
  TEST_METHOD(MainTestMethod) {
    //https://stackoverflow.com/questions/25150708/how-to-get-the-projectdir-path-in-a-visual-studio-native-c-unit-testing-usi
    std::string s = EXPAND(UNITTESTPRJ);
    s.erase(0, 1); // erase the first quote
    s.erase(s.size() - 2); // erase the last quote and the dot
    std::string my_project_dir = s;

    // resources
    const char* cubepath = "res/3dmodels/cube.glb";
    std::string fullcubepath = s + cubepath;
    const char* groundplane = "res/3dmodels/large_green_plane.obj";
    std::string fullgroundplane = s + groundplane;
    const char* peasant_man = "res/3dmodels/peasant_man.dae";
    std::string fullpeasant_man = s + peasant_man;

    // init engine
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
    static int cube_id = instance.AddProp(fullcubepath.c_str(), glm::vec3(0, 0, -10));
    static int groundplane_id = instance.AddProp(fullgroundplane.c_str(), glm::vec3(0, -30.f, 0));
    instance.ScaleProp(groundplane_id, glm::vec3(2));
    static int peasant_man_id = instance.AddProp(fullpeasant_man.c_str(), glm::vec3(0, -30, -100));
    instance.ScaleProp(peasant_man_id, glm::vec3(.25f));

    instance.AddToUpdate([](float dt) {
      static float accum_time = 0;
      accum_time += dt;
      instance.RotateProp(cube_id, glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      });

    // default light and background
    instance.SetWindowClearColor();
    static float lightdir[3] = { -.3f,-.3f,-.3f };
    static float* lightamb = new float(.1f);
    static float* lightdiff = new float(.6f);
    static float* lightspec = new float(.6f);
    instance.SetDirectionalLight(
      glm::vec3(lightdir[0], lightdir[1], lightdir[2]),
      glm::vec3(*lightamb),
      glm::vec3(*lightdiff),
      glm::vec3(*lightspec));
    static bool No = false;
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Model Test");
      ImGui::SliderFloat3("Light Direction", lightdir, -1.f, 1.f, "%f", 1.0f);
      ImGui::SliderFloat("Light Ambient", lightamb, 0.f, 1.f, "%f", 1.0f);
      ImGui::SliderFloat("Light Diffuse", lightdiff, 0.f, 1.f, "%f", 1.0f);
      ImGui::SliderFloat("Light Spec", lightspec, 0.f, 1.f, "%f", 1.0f);
      bool update_light = ImGui::Button("UpdateLight");
      if (update_light)
        instance.SetDirectionalLight(
          glm::vec3(lightdir[0], lightdir[1], lightdir[2]),
          glm::vec3(*lightamb),
          glm::vec3(*lightdiff),
          glm::vec3(*lightspec));
      ImGui::Text("Do you see the spinning cube?");
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

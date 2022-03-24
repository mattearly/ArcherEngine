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
    const char* walking_man = "3dmodels/Walking.dae";
    std::string fullwalking_man = runtime_dir + walking_man;

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
    instance.StencilProp(cube_id, true);
    instance.StencilPropColor(cube_id, glm::vec3(.4, .4, .4));
    instance.StencilPropWithNormals(cube_id, false);
    instance.StencilPropScale(cube_id, 1.1f);

    static int cube_id2 = instance.AddProp(fullcubepath.c_str(), glm::vec3(-10, 0, -10));
    static int cube_id3 = instance.AddProp(fullcubepath.c_str(), glm::vec3(10, 0, -10));


    static int groundplane_id = instance.AddProp(fullgroundplane.c_str(), glm::vec3(0, -30.f, 0));
    instance.ScaleProp(groundplane_id, glm::vec3(3,1,3));


    static int peasant_man_id = instance.AddProp(fullpeasant_man.c_str(), glm::vec3(0, -30, -100));
    instance.ScaleProp(peasant_man_id, glm::vec3(.25f));
    instance.StencilProp(peasant_man_id, true);
    instance.StencilPropColor(peasant_man_id, glm::vec3(.4,.4,.4));
    instance.StencilPropWithNormals(peasant_man_id, true);
    instance.StencilPropScale(peasant_man_id, 3.f);

    static int walking_man_id = instance.AddProp(fullwalking_man.c_str(), glm::vec3(-60, -30, -100));
    instance.ScaleProp(walking_man_id, glm::vec3(.25f));
    instance.StencilProp(walking_man_id, true);
    instance.StencilPropColor(walking_man_id, glm::vec3(.4,.4,.4));
    instance.StencilPropWithNormals(walking_man_id, true);
    instance.StencilPropScale(walking_man_id, 3.f);

    instance.AddToUpdate([](float dt) {
      static float accum_time = 0;
      accum_time += dt;
      instance.RotateProp(cube_id, glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      instance.RotateProp(cube_id2, glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      instance.RotateProp(cube_id3, glm::vec3(cos(accum_time), sin(accum_time), sin(accum_time)));
      });

    // default light and background
    instance.SetWindowClearColor();
    static float lightdir[3] = { .21f, .21f, .1f };
    static float* lightamb = new float(.2f);
    static float* lightdiff = new float(.7f);
    static float* lightspec = new float(.7f);
    instance.SetDirectionalLight(
      glm::vec3(lightdir[0], lightdir[1], lightdir[2]),
      glm::vec3(*lightamb),
      glm::vec3(*lightdiff),
      glm::vec3(*lightspec));
    static bool No = false;
    instance.AddToImGuiUpdate([]() {
      ImGui::Begin("Model Test");
      bool update_light1 = ImGui::SliderFloat3("Light Direction", lightdir, -1.f, 1.f, "%f", 1.0f);
      bool update_light2 = ImGui::SliderFloat("Light Ambient", lightamb, 0.f, 1.f, "%f", 1.0f);
      bool update_light3 = ImGui::SliderFloat("Light Diffuse", lightdiff, 0.f, 1.f, "%f", 1.0f);
      bool update_light4 = ImGui::SliderFloat("Light Spec", lightspec, 0.f, 1.f, "%f", 1.0f);
      ImGui::Text("Does everything look right?");
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

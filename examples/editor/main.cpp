// this example sets up a base run with drag and drop enable.
// drop in a model file to see it. this is not a finished example.

#include "fpp_fly.h"
#include "file_menu.h"
static AA::Interface engine{};
static unsigned int editor_cam{};
int main() {

  // local stuff for init, in it's own scope because we have no reason
  // to keep this stuff in memory after the init is done. optionally
  // this could all go in a function.
  {
    AA::WindowOptions winopts;
    winopts._windowing_mode = AA::WINDOW_MODE::MAXIMIZED;
    winopts._editor_drag_and_drop = true;
    winopts._title = "AA Editor";
    if (!engine.Init(winopts)) return -1;
    engine.SetLogStream(true, false);  // log to console

    editor_cam = engine.AddCamera();  // add camera
    engine.SetSunLight(glm::vec3(-.33f, -1.f, -.33f), glm::vec3(.8f), glm::vec3(0.1f), glm::vec3(0.5f));
    auto strong_cam_ref = engine.GetCamera(editor_cam).lock();
    strong_cam_ref->SetPosition(glm::vec3(0.0f, 65.0f, -65.f));
    strong_cam_ref->SetYaw(90.f);
    strong_cam_ref->SetPitch(-35.f);
    engine.GetSunLight().lock()->Shadows = false;  // no shadows
    setup_fpp_fly(editor_cam, engine);
    setup_file_menu(engine);
    engine.AddToOnQuit([]() { turn_off_fly(); remove_file_menu(); });

  }

  return engine.Run();
}

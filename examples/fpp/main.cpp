#include "fpp_fly.h"
static AA::Interface engine{};
static unsigned int editor_cam{};
int main() {
  {
    AA::WindowOptions winopts;
    winopts._windowing_mode = AA::WINDOW_MODE::MAXIMIZED;
    winopts._editor_drag_and_drop = true;
    winopts._title = "AA Editor";
    if (!engine.Init(winopts)) return -1;
    editor_cam = engine.AddCamera();
    engine.SetLogStream(true, false);
    engine.SetSunLight(glm::vec3(-.33f, -1.f, -.33f), glm::vec3(.1f), glm::vec3(0.6f), glm::vec3(0.9f));
  }


  // turn on log stream to console since we have our console going




  // default cam settings
  {
    auto strong_cam_ref = engine.GetCamera(editor_cam).lock();
    strong_cam_ref->SetPosition(glm::vec3(0.0f, 65.0f, -65.f));
    strong_cam_ref->SetYaw(90.f);
    strong_cam_ref->SetPitch(-35.f);

     // default sun and shadows settings
    {
      auto strong_sunlight_ref = engine.GetSunLight().lock();
      strong_sunlight_ref->ShadowOrthoSize = (strong_cam_ref->GetRenderDistance() + 200.f) / 2.f;
      strong_sunlight_ref->ShadowFarPlane = strong_cam_ref->GetRenderDistance()+200.f;
    }
  }
  setup_fpp_fly(editor_cam, engine);

  engine.AddToOnQuit([]() { turn_off_fly(); });

  return engine.Run();
}

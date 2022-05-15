#include <AAEngine/Interface.h>
#include "fly_cam_script.h"
static AA::Interface engine{};
static unsigned int editor_cam{};
int main() {
  {
    AA::WindowOptions winopts;
    winopts._windowing_mode = AA::WINDOW_MODE::MAXIMIZED;
    winopts._editor_drag_and_drop = true;
    winopts._title = "AA Editor";
    if (!engine.Init(winopts)) return -1;
  }

  engine.SetSunLight(glm::vec3(-.33f, -1.f, -.33f), glm::vec3(.1f), glm::vec3(0.6f), glm::vec3(0.9f));


  editor_cam = engine.AddCamera();
  {  // default cam settings
    auto weak = engine.GetCamera(editor_cam);
    auto strong = weak.lock();
    strong->SetPosition(glm::vec3(0.0f, 40.0f, -40.f));
    strong->SetYaw(90.f);
    strong->SetPitch(-35.f);

    { // default sun and shadows settings
      auto weakd = engine.GetSunLight();
      auto strongd = weakd.lock();
      strongd->ShadowOrthoSize = (strong->GetRenderDistance()+200.f)/2.f;
      strongd->ShadowFarPlane = strong->GetRenderDistance()+200.f;

    }
  }
  setup_fpp_fly(editor_cam, engine);
  
  engine.AddToOnQuit([]() { turn_off_fly(); });

  return engine.Run();
}

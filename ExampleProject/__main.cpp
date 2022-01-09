/** Example Project
* What is this?  Just me playing around with a sample usage of AAEngine. - Matt
* */

#include "Manager.h"

AA::Interface g_engine;
SCENE g_next_scene;

static bool init_aa_engine_helper();

int main(int argc, char** argv) {
  if (!init_aa_engine_helper()) {
    return -999;
  }
  static Manager manager;
  g_engine.AddToUpdate([](float dt) {
    manager.tick(dt);
  });
  return g_engine.Run();
}

static bool init_aa_engine_helper() {
  AA::WindowOptions winopts;
  winopts._title = "ExampleProject";
#ifdef _DEBUG
  winopts._windowing_mode = AA::WINDOW_MODE::WINDOWED_DEFAULT;
#else
  winopts._windowing_mode = AA::WINDOW_MODE::FULLSCREEN_BORDERLESS;
#endif
  return g_engine.Init(winopts);
}
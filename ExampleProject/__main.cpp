/** Example Project
* What is this?  Just me playing around with a sample usage of AAEngine. - Matt
* */

#include <AncientArcher/AncientArcher.h>
#include <AncientArcher/WindowOptions.h>
#include "Manager.h"

AA::AncientArcher Engine;

int main(int argc, char** argv) {

  AA::WindowOptions winopts;
  winopts._title = "ExampleProject";
#ifdef _DEBUG
  winopts._windowing_mode = AA::WINDOW_MODE::WINDOWED_DEFAULT;
#else
  winopts._windowing_mode = AA::WINDOW_MODE::FULLSCREEN;
#endif
  Engine.Init(winopts);

  static Manager manager;
  Engine.AddToUpdate([](float dt){
    manager.tick(dt);
  });

  return Engine.Run();
}

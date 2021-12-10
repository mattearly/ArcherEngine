/** Example Project
* What is this?  Just me playing around with a sample usage of AAEngine. - Matt
* */

#include <AncientArcher/AncientArcher.h>
#include "game/MainGame.h"

MainGame myGame;
AA::AncientArcher instance;

int main(int argc, char** argv) {
  instance.Init();
  instance.SetWindowTitle("ExampleProject");
  myGame.Setup();
  instance.AddToUpdate([](float dt) { myGame.Update(dt); });
  instance.AddToOnTeardown([]() { myGame.Teardown(); });
  return instance.Run();
}

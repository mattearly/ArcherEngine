#pragma once
#include <AncientArcher/AncientArcher.h>
#include "MainCharacter.h"
#include "setup/setskybox.h"
#include "setup/setmodels.h"
#include "setup/setdirlight.h"
#include "setup/setfullscreentoggle.h"

MainCharacter my_character;

struct MainGame {

  void Setup() {
    my_character.Setup();
    setmodels();
    setskybox();
    setdirlight();
    setfullscreentoggle();
  };

  void Update(float dt) {
    my_character.Update(dt);
  };

  void Teardown() {
    my_character.Teardown();
  };

};
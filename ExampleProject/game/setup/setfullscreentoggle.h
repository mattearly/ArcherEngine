#pragma once
#include "AncientArcher/AncientArcher.h"
extern AA::AncientArcher instance;
void setfullscreentoggle() {
  static float full_screen_toggle_timer = 0.f;
  instance.AddToKeyHandling([](AA::KeyboardButtons& kb) {
    const float cd = .7f;
    // toggle fullscreen if alt+enter (or f11) and cd ready
    if (((kb.enter && (kb.leftAlt || kb.rightAlt)) || kb.f11) && full_screen_toggle_timer > cd) {
      instance.ToggleFullscreen(true);
      full_screen_toggle_timer = 0.f;  // reset timer on activation
    }
  });
  instance.AddToUpdate([](float dt){
    full_screen_toggle_timer += dt;  // fullscreen timer
  });
}
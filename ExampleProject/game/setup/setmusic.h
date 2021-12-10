#pragma once
#include <AncientArcher/AncientArcher.h>
#include <iostream>

extern AA::AncientArcher instance;

//const char* town_theme_stereo = "E:\\AssetPack\\TownTheme_stereo.wav";
const char* talking_stereo = "res/talking_stereo.wav";

bool is_music_setup = false;
void setmusic() {
  if (is_music_setup)
    return;

  is_music_setup = true;
  instance.AddMusic(talking_stereo);
  int id = instance.AddToKeyHandling([](AA::KeyboardButtons& kb) {
    if (kb.p) {         // call to play music
      instance.PlayMusic();
    } else if (kb.o) {  // call to stop the music
      instance.StopMusic();
    } else if (kb.i) {  // call to pause
      instance.PauseMusic();
    } else if (kb.k) {  // call to resume (from pause)
      instance.ResumeMusic();
    } else if (kb.u) {  // call to remove (unload) music
      instance.RemoveMusic();
    } else if (kb.r) {  // call to add (load) music
      instance.AddMusic(talking_stereo);
    }
  });

  static float the_vol = 1.f;
  id = instance.AddToScrollHandling([](AA::MouseScrollWheel& si) {
    if (si.yOffset > 0) {
      the_vol += .1f;
      if (the_vol > 1.f)
        the_vol = 1.f;
      instance.SetMusicVolume(the_vol);
    }
    if (si.yOffset < 0) {
      the_vol -= .1f;
      if (the_vol < .0f)
        the_vol = .0f;
      instance.SetMusicVolume(the_vol);
    }
  });

}

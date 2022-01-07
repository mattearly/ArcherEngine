//#pragma once
//#include "../Manager.h"
//
//const char* talking_stereo = "res/talking_stereo.wav";
//
//bool is_music_setup = false;
//void setmusic() {
//  if (is_music_setup)
//    return;
//
//  is_music_setup = true;
//  Engine.AddMusic(talking_stereo);
//  int id = Engine.AddToKeyHandling([](AA::KeyboardButtons& kb) {
//    if (kb.p) {         // call to play music
//      Engine.PlayMusic();
//    } else if (kb.o) {  // call to stop the music
//      Engine.StopMusic();
//    } else if (kb.i) {  // call to pause
//      Engine.PauseMusic();
//    } else if (kb.k) {  // call to resume (from pause)
//      Engine.ResumeMusic();
//    } else if (kb.u) {  // call to remove (unload) music
//      Engine.RemoveMusic();
//    } else if (kb.r) {  // call to add (load) music
//      Engine.AddMusic(talking_stereo);
//    }
//  });
//
//  static float the_vol = 1.f;
//  id = Engine.AddToScrollHandling([](AA::MouseScrollWheel& si) {
//    if (si.yOffset > 0) {
//      the_vol += .1f;
//      if (the_vol > 1.f)
//        the_vol = 1.f;
//      Engine.SetMusicVolume(the_vol);
//    }
//    if (si.yOffset < 0) {
//      the_vol -= .1f;
//      if (the_vol < .0f)
//        the_vol = .0f;
//      Engine.SetMusicVolume(the_vol);
//    }
//  });
//
//}

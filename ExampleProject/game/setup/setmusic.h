//#pragma once
//#include <AncientArcher/AncientArcher.h>
//#include <iostream>
////const char* town_theme_stereo = "E:\\AssetPack\\TownTheme_stereo.wav";
//const char* talking_stereo = "..\\ExampleProject\\res\\talking_stereo.wav";
//
//bool is_music_setup = false;
//void setmusic() {
//  if (is_music_setup)
//    return;
//
//  is_music_setup = true;
//  AA::AddMusic(talking_stereo);
//  AA::AddToTimedOutKeyHandling([](AA::KeyboardInput& kb) -> bool {
//    if (kb.p) {         // call to play music
//      AA::PlayMusic();
//      return true;
//    } else if (kb.o) {  // call to stop the music
//      AA::StopMusic();
//      return true;
//    } else if (kb.i) {  // call to pause
//      AA::PauseMusic();
//      return true;
//    } else if (kb.k) {  // call to resume (from pause)
//      AA::ResumeMusic();
//      return true;
//    } else if (kb.u) {  // call to remove (unload) music
//      AA::RemoveMusic();
//      return true;
//    } else if (kb.r) {  // call to add (load) music
//      AA::AddMusic(talking_stereo);
//      return true;
//    }
//    return false;
//  });
//
//  static float the_vol = 1.f;
//  AA::AddToScrollHandling([](AA::ScrollInput& si) {
//    if (si.yOffset > 0) {
//      the_vol += .1f;
//      if (the_vol > 1.f)
//        the_vol = 1.f;
//      AA::SetMusicVolume(the_vol);
//#ifdef _DEBUG
//      std::cout << "music vol increased to " << the_vol << "\n";
//#endif
//    }
//    if (si.yOffset < 0) {
//      the_vol -= .1f;
//      if (the_vol < .0f)
//        the_vol = .0f;
//      AA::SetMusicVolume(the_vol);
//#ifdef _DEBUG
//      std::cout << "music vol decreased to " << the_vol << "\n";
//#endif
//    }
//  });
//
//}
// pch.cpp: source file corresponding to the pre-compiled header

#include "pch.h"
AA::Interface instance;
// When you are using pre-compiled headers, this source file is necessary for compilation to succeed.
std::string getProjDir() {
  //https://stackoverflow.com/questions/25150708/how-to-get-the-projectdir-path-in-a-visual-studio-native-c-unit-testing-usi
  std::string s = EXPAND(UNITTESTPRJ);
  s.erase(0, 1); // erase the first quote
  s.erase(s.size() - 2); // erase the last quote and the dot
  std::string my_project_dir = s;
  return s;
}
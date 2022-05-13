#include <AAEngine/Interface.h>
static AA::Interface aaclient; 
int main() {
  {
    AA::WindowOptions winopts;
    winopts._windowing_mode = AA::WINDOW_MODE::MAXIMIZED;
    winopts._title = "AA Editor";
    if (!aaclient.Init(winopts)) return -1;
  }
  return aaclient.Run();
}
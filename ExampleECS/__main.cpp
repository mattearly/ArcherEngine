#include <AncientArcher/AncientArcher.h>

using namespace AA;
AncientArcher instance;

int main(int argc, char** argv) {
  instance.Init();

  return instance.Run();
}
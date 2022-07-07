#pragma once
namespace AA {
class PrimativeMaker {
public:
  static unsigned int load_cone(unsigned int& out_num_elements);
  static void unload_cone();
  static unsigned int load_cube();
  static void unload_cube();
  static unsigned int load_plane();
  static void unload_plane();
  static void unload_all();
private:
  PrimativeMaker() = delete;
};
}


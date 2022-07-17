// only OpenGL class can use this extension
#pragma once
namespace AA {
class PrimativeMaker {
private:
  friend class OpenGL;
  static unsigned int load_cone();
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


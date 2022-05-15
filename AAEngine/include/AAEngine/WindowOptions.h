#pragma once
#include <string>
#include "Version.h"
namespace AA {
enum class WINDOW_MODE { FULLSCREEN, WINDOWED, WINDOWED_DEFAULT, FULLSCREEN_BORDERLESS, MAXIMIZED };
enum class RENDER_TECH { OPENGL4, D3D11, VULKAN1 };
// hidden is the same as normal, just not drawn
// disabled is raw motion (doesn't apply desktop cursor settings) - check if it is supported with glfwRawMouseMotionSupported()
enum class CURSOR_MODE { HIDDEN = 0x00034002, DISABLED = 0x00034003, NORMAL = 0x00034001 };  //glfw hidden, disabled, normal

struct WindowOptions final {
  // min size is the size of a gameboy screen http://www2.hawaii.edu/~dkm/project2/color.html#:~:text=The%20screen%20resolution%20was%20the,communications%20port%20for%20wireless%20linking.
  static constexpr int   ENGINE_MIN_WIDTH = 160, ENGINE_MIN_HEIGHT = 144;
  int          _min_width = ENGINE_MIN_WIDTH, _min_height = ENGINE_MIN_HEIGHT;
  int          _width = 800, _height = 600;
  std::string  _title = "default title";
  WINDOW_MODE  _windowing_mode = WINDOW_MODE::WINDOWED_DEFAULT;
  RENDER_TECH  _rendering_tech = RENDER_TECH::OPENGL4;
  CURSOR_MODE  _cursor_mode = CURSOR_MODE::NORMAL;
  int          _msaa_samples = -1;
  unsigned int _stencil_bits = 8;
  bool         _vsync = false;
  bool         _gamma_correction = false;
  bool         _editor_drag_and_drop = false;
};

static void SetDefaults(WindowOptions& winopts) {
  winopts._min_width = WindowOptions::ENGINE_MIN_WIDTH;
  winopts._min_height = WindowOptions::ENGINE_MIN_HEIGHT;
  winopts._width = 800;
  winopts._height = 600;
  winopts._title = "AncientArcher v" + std::to_string(ENGINEVERSIONMAJOR) + '.' + std::to_string(ENGINEVERSIONMINOR) + '.' + std::to_string(ENGINEVERSIONPATCH);
  winopts._windowing_mode = WINDOW_MODE::WINDOWED_DEFAULT;
  winopts._rendering_tech = RENDER_TECH::OPENGL4;
  winopts._cursor_mode = CURSOR_MODE::NORMAL;
  winopts._msaa_samples = -1;  // GLFW_DONT_CARE
  winopts._stencil_bits = 8;
  winopts._vsync = false;
  winopts._gamma_correction = false;
  winopts._editor_drag_and_drop = false;
}
}  // end namespace

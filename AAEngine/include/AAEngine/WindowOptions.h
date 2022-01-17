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
  int          _width = 800, _height = 600;
  std::string  _title = "default title";
  WINDOW_MODE  _windowing_mode = WINDOW_MODE::WINDOWED_DEFAULT;
  RENDER_TECH  _rendering_tech = RENDER_TECH::OPENGL4;
  CURSOR_MODE  _cursor_mode = CURSOR_MODE::NORMAL;
  int          _msaa_samples = -1;
  unsigned int _stencil_bits = 8;
  bool         _vsync = false;
};

static void SetDefaults(WindowOptions& winopts) {

  winopts._width = 800;
  winopts._height = 600;
  winopts._title = "AncientArcher v" + std::to_string(ENGINEVERSIONMAJOR) + '.' + std::to_string(ENGINEVERSIONMINOR) + '.' + std::to_string(ENGINEVERSIONPATCH);
  winopts._windowing_mode = WINDOW_MODE::WINDOWED;
  winopts._rendering_tech = RENDER_TECH::OPENGL4;
  winopts._cursor_mode = CURSOR_MODE::NORMAL;
  winopts._msaa_samples = -1;  // GLFW_DONT_CARE
  winopts._stencil_bits = 8;
  winopts._vsync = false;

}
}  // end namespace

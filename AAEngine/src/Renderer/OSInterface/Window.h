#pragma once
#include "../../../include/AncientArcher/Controls/Input.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>
#include <memory>
#include <string>

namespace AA {


enum class WINDOW_MODE { FULLSCREEN, WINDOWED, FULLSCREEN_BORDERLESS, MAXIMIZED };
enum class RENDER_TECH { OPENGL4, D3D11, VULKAN1 };
// hidden is the same as normal, just not drawn
// disabled is raw motion (doesn't apply desktop cursor settings) - check if it is supported with glfwRawMouseMotionSupported()
enum class CURSOR_MODE { HIDDEN = 0x00034002, DISABLED = 0x00034003, NORMAL = 0x00034001 };  //glfw hidden, disabled, normal
struct WindowOptions final {
  WindowOptions();
  int _width, _height;
  std::string _title;
  WINDOW_MODE _windowing_mode;
  RENDER_TECH _rendering_tech;
  CURSOR_MODE _cursor_mode;
  int _msaa_samples;  // GLFW_DONT_CARE
  bool _vsync;
};

class Window final {
public:

  Window();
  Window(const Window& window);
  ~Window();
  
  std::shared_ptr<WindowOptions> GetModifiableWindowOptions();
  void ApplyChanges();


  void SetCursorToHidden() noexcept;
  void SetCursorToDisabled() noexcept;
  void SetCursorToNormal() noexcept;

  void Close();

  bool GetShouldClose();
  int GetCurrentWidth();
  int GetCurrentHeight();

private:

  void default_init();
  void default_init(const Window& window);
  void set_default_callbacks();

  GLFWwindow* mGLFWwindow;

  std::shared_ptr<WindowOptions> mWindowOptions;

  // used for comparisons when applying new changes, updates on GetModifiableWindowOptions
  WindowOptions prev_window_options;

  // @status[in]: force or unforce glfw window into/out of fullscreen mode
  void set_window_fullscreen(const bool status) noexcept;

  void clear_screen();
  void swap_buffers();

  bool settings_applied_at_least_once = false;

  // any callback that casts the glfwwindow to user_ptr and then accesses private members needs to be a friend
  friend void FRAMEBUFFERSIZESETCALLBACK(GLFWwindow* window, int w, int h);
  friend void ONWINDOWFOCUSCALLBACK(GLFWwindow* window, int focused);

  // the Instance class is a controller of this class
  friend class AncientArcher;

};

}  // end namespace AA

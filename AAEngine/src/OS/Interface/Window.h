#pragma once
#include "../../../include/AAEngine/Controls/Input.h"
#include "../../../include/AAEngine/WindowOptions.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>
#include <memory>
#include <string>

namespace AA {


class Window final {
public:

  Window();
  Window(WindowOptions winopts);
  Window(std::shared_ptr<WindowOptions> winopts);
  //Window(const Window& window); // todo: shared context
  ~Window();
  
  std::shared_ptr<WindowOptions> GetModifiableWindowOptions();
  void ApplyChanges();

  void SetViewportToWindowSize() noexcept;

  void SetCursorToHidden() noexcept;
  void SetCursorToDisabled() noexcept;
  void SetCursorToNormal() noexcept;

  void Close();

  bool GetShouldClose();
  int GetCurrentWidth();
  int GetCurrentHeight();

private:

  void default_init();
  //void default_init(const Window& window);
  void set_default_callbacks();

  GLFWwindow* mGLFWwindow;

  std::shared_ptr<WindowOptions> mWindowOptions;

  // used for comparisons when applying new changes, updates on GetModifiableWindowOptions
  WindowOptions prev_window_options;

  void apply_window_sizings_from_current_options() noexcept;

  //void clear_screen();
  void swap_buffers();

  bool settings_applied_at_least_once = false;

  // any callback that casts the glfwwindow to user_ptr and then accesses private members needs to be a friend
  friend void FRAMEBUFFERSIZESETCALLBACK(GLFWwindow* window, int w, int h);
  friend void ONWINDOWFOCUSCALLBACK(GLFWwindow* window, int focused);

  // the Instance class is a controller of this class
  friend class Interface;

};

}  // end namespace AA

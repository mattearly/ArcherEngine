#pragma once
#include "../../Controls/Input.h"
#include "../../WindowOptions.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <utility>
#include <memory>
#include <string>
#include <queue>

namespace AA {

class Window final {
public:

  Window();
  Window(WindowOptions winopts);
  Window(std::shared_ptr<WindowOptions> winopts);
  ~Window();

  /// <summary>
  /// Hides the mouse cursor. 
  /// Used for when you are drawing your own cursor.
  /// </summary>
  void SetCursorToHidden() noexcept;

  /// <summary>
  /// Disables the cursor. See disabled in glfw documentation. https://www.glfw.org/docs/3.3/input_guide.html
  /// </summary>
  void SetCursorToDisabled() noexcept;

  /// <summary>
  /// Sets the Cursor the the standard OS Cursor.
  /// </summary>
  void SetCursorToNormal() noexcept;

  bool GetShouldClose();

  int GetCurrentWidth();
  int GetCurrentHeight();

  void SetCurrentWidthAndHeight(int w, int h) noexcept;
  void SetMinWidthAndHeight(int w, int h) noexcept;

  int GetCurrentMinWidth() const;
  int GetCurrentMinHeight() const;

  void SetGammaCorrection(const bool enabled);
  bool GetGammaCorrection() const;

  void Close();

  void SetDragAndDrop(const bool enabled);
  bool IsDragAndDropEnabled() const;
private:

  std::weak_ptr<WindowOptions> get_and_note_window_options();
  void apply_new_window_option_changes();
  void default_init();
  void set_default_callbacks();
  void apply_window_sizings_from_current_options() noexcept;

  GLFWwindow* mGLFWwindow = nullptr;
  std::shared_ptr<WindowOptions> mWindowOptions;
  WindowOptions mPrevWindowOptions;    // used for comparisons when applying new window options changes
  bool settings_applied_at_least_once = false;

  // any callback that casts the glfwwindow to user_ptr and then accesses private members needs to be a friend
  friend void FRAMEBUFFERSIZESETCALLBACK(GLFWwindow* window, int w, int h);
  friend void ONWINDOWFOCUSCALLBACK(GLFWwindow* window, int focused);
  friend void ONDRAGANDDROP(GLFWwindow* window, int count, const char** paths);

  friend class Interface;  // to call swap_buffers when needed, todo: come up with a better solution, as we don't really want the interface to have full access to the window but it is okay for now.
  void swap_buffers();
  std::queue<std::string> dropped_paths;
};

}  // end namespace AA

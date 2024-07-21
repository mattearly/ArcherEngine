#pragma once

#ifdef __linux__
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#elif _WIN32
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

namespace AA {
class imGUI {
public:
  imGUI();
  void InitOpenGL(GLFWwindow* window);
  void Shutdown();
  void NewFrame();
  //void Update();  // updates done elsewhere
  void Render();
private:
  bool isInit;
};

} // end namespace AA

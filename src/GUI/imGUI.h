#pragma once
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
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

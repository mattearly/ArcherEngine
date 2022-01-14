#include "imGUI.h"
#include "../OS/OpenGL/OGLShader.h"
#include "../OS/OpenGL/OGLGraphics.h"
namespace AA {
imGUI::imGUI() {
  isInit = false;
}
// run InitOpenGL only after AA::Init() has been called (or
void imGUI::InitOpenGL(GLFWwindow* window) {
  if (!isInit) {
    // todo: use return types and set isInit with more 'thought'
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(OGLShader::get_glsl_version());
    isInit = true;
  }
}

void imGUI::Shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void imGUI::NewFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

//void imGUI::Update() {
  //ImGui::Text("Hello, world!");
  //static float color[4] = { 1.f, 1.f, 1.f, 1.f };
  //ImGui::ColorEdit3("color", color);
//}

void imGUI::Render() {
  OGLGraphics::SetDepthTest(false);
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  OGLGraphics::SetDepthTest(true);
}


}

#include <Interface.h>
#include <Scene/Camera.h>
#include <OS/Interface/Window.h>
#include <Scene/Scene.h>
#include "OS/OpenGL/Graphics.h"
#include "OS/OpenGL/Loaders/PrimativeMaker.h"
#include "Physics/NVidiaPhysx.h"
#include "Scene/Lights.h"
#include "Scene/Skybox.h"
#include "Sound/SoundDevice.h"
#include "Sound/Speaker.h"
#include "Sound/SoundEffect.h"
#include "Sound/LongSound.h"
#include "GUI/imGUI.h"
#include "OS/OpenGL/InternalShaders/Init.h"
#include <string>
#include <sstream>
#include <utility>
#include <algorithm>
#include <unordered_map>

namespace AA {

extern void g_poll_input_events();
extern bool g_new_key_reads;
extern bool g_os_window_resized;
extern KeyboardButtons g_keyboard_input_status;
extern MouseScrollWheel g_scroll_input_status;
extern MouseCursorPos g_mouse_input_status;
extern MouseButtons g_mouse_button_status;

// runs once after a new run start
void Interface::begin() {
  for (const auto& oB : onStart) {
    oB.second();
  }
}

// Runs core and all user defined functionality
void Interface::update() {

  // init delta clock on first tap into update
  static float currTime;
  static float lastTime = (float)glfwGetTime();
  float elapsedTime;

  // update engine run delta times
  currTime = (float)glfwGetTime();
  elapsedTime = currTime - lastTime;
  lastTime = currTime;

  for (auto& oDU : onUpdate) {
    oDU.second(elapsedTime);
  }

  if (mSimulateWorldPhysics) {
    NVidiaPhysx::Get()->StepPhysics(elapsedTime);
  }

  for (auto& p : mProps) {
    p->Update();
    if (p->animdata_)
      p->UpdateAnim(elapsedTime);
  }

  if (mMusic) {
    static float music_rebuffer_cd = 0;
    music_rebuffer_cd += elapsedTime;
    if (music_rebuffer_cd > .5f) {  // todo: math with file size and stuff to figure out how long this cd should actually be
      mMusic->UpdatePlayBuffer();
      music_rebuffer_cd = 0;
    }
  }

  if (g_new_key_reads) {
    for (auto& oKH : onKeyHandling) { oKH.second(g_keyboard_input_status); }
    for (auto& oMH : onMouseButtonHandling) { oMH.second(g_mouse_button_status); }
    for (auto& oSH : onScrollHandling) { oSH.second(g_scroll_input_status); }
    g_scroll_input_status.yOffset = g_scroll_input_status.xOffset = 0;
    g_new_key_reads = false;
  }

  for (auto& oMH : onMouseHandling) { oMH.second(g_mouse_input_status); }
}

void Interface::settle_window_resize_flag() {
  if (!mCameras.empty()) {
    for (auto& cam : mCameras) {
      if (cam->GetIsAlwaysScreenSize()) {
        cam->SetBottomLeft(0, 0);
        cam->SetDimensions(mWindow->GetCurrentWidth(), mWindow->GetCurrentHeight());
      }
      cam->ProjectionChanged();
    }
  }
  g_os_window_resized = false;
}

void Interface::pre_render() {
  if (g_os_window_resized) {
    settle_window_resize_flag();
  }
  OpenGL::GetGL()->NewFrame();

}

// Renders visable props every frame
void Interface::render() {
  if (!mCameras.empty()) {
    if (mSunLight) { OpenGL::GetGL()->BatchRenderShadows(mCameras.front()->GetPosition(), *mSunLight, mProps/*, mAnimatedProps*/); }
    for (const auto& cam : mCameras) {
      cam->NewFrame();
      OpenGL::GetGL()->RenderSkybox(cam->GetSkybox(), cam->GetViewport());
      OpenGL::GetGL()->BatchRenderToViewport(mProps, /*mAnimatedProps, */cam->GetViewport());
      if (mDebugLightIndicators) {
        for (const auto& pl : mPointLights)
          OpenGL::GetGL()->RenderDebugCube(pl->Position);
        //for (const auto& sl : mSpotLights)
        //  OpenGL::GetGL()->RenderSpotLightIcon(sl->Position, sl->Direction);
        //if (mSunLight)
        //  OpenGL::GetGL()->RenderDirectionalLightArrowIcon(mSunLight->Direction);
      }
    }
  }

  // render imgui interface
  if (mIMGUI) {
    mIMGUI->NewFrame();
    for (auto& oIU : onImGuiUpdate) { oIU.second(); }
    mIMGUI->Render();
  }
}

void Interface::post_render() {
  mWindow->swap_buffers();
  g_poll_input_events();
  if (mWindow) if (mWindow->mWindowOptions->_editor_drag_and_drop) if (!mWindow->dropped_paths.empty()) {
    AddProp(mWindow->dropped_paths.front().c_str(), false);  // process queue item
    mWindow->dropped_paths.pop();  // remove from queue
  }
}

// Runs Once on Engine Shutdown
void Interface::teardown() {
  // run user preferred functions first
  for (auto& oTD : onQuit) {
    oTD.second();
  }

  mCameras.clear();

  PrimativeMaker::unload_all();

  ClearAllRuntimeLamdaFunctions();

  InternalShaders::Shutdown();

  // delete all the meshes and textures from GPU memory
  mProps.clear();

  for (auto& anim : mAnimation) {
    anim.reset();
  }
  mAnimation.clear();

  mSunLight.reset();
  mPointLights.clear();
  mSpotLights.clear();
  mDebugLightIndicators = false;

  // delete imgui
  if (mIMGUI) {
    mIMGUI->Shutdown();
    delete mIMGUI;
    mIMGUI = nullptr;
  }

  mSpeakers.clear();

  mSoundEffects.clear();

  if (mMusic) {
    RemoveMusic();
  }

  SoundDevice::Shutdown();

  mWindow.reset();

  NVidiaPhysx::Shutdown();

  isInit = false;
}

}  // end namespace AA
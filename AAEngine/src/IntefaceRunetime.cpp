#include "../include/AAEngine/Interface.h"
#include "../include/AAEngine/Scene/Camera.h"
#include "../include/AAEngine/OS/Interface/Window.h"
#include "OS/OpenGL/OGLGraphics.h"
#include "OS/OpenGL/InternalShaders/Stencil.h"
#include "OS/OpenGL/InternalShaders/Uber.h"
#include "OS/OpenGL/InternalShaders/Shadow.h"
#include "Physics/NVidiaPhysx.h"
#include "../include/AAEngine/Mesh/Prop.h"
#include "../include/AAEngine/Mesh/AnimProp.h"
#include "Scene/Lights.h"
#include "Scene/Skybox.h"
#include "Sound/SoundDevice.h"
#include "Sound/Speaker.h"
#include "Sound/SoundEffect.h"
#include "Sound/LongSound.h"
#include "GUI/imGUI.h"
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
  g_poll_input_events();

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
    p->spacial_data.ProcessModifications();
  }

  for (auto& ap : mAnimProps) {
    ap->spacial_data.ProcessModifications();
    if (ap->mAnimator) {
      ap->UpdateAnim(elapsedTime);
    }
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

// Renders visable props every frame
void Interface::render() {
  if (mCameras.empty())
    return;

  OGLGraphics::ClearScreen();
  OGLGraphics::SetDepthTest(true);
  OGLGraphics::SetDepthMode(GL_LESS);
  InternalShaders::Uber::Get()->SetBool("u_is_animating", false);
  InternalShaders::Stencil::Get()->SetBool("u_is_animating", false);
  
  // draw to all cameras
  for (auto& cam : mCameras) {
    if (cam->isAlwaysScreenSize) {
      if (g_os_window_resized) {
        cam->SetBottomLeft(0, 0);
        cam->SetDimensions_testing(mWindow->GetCurrentWidth(), mWindow->GetCurrentHeight());
        cam->updateProjectionMatrix();
        g_os_window_resized = false;
      }
    }
    cam->updateViewMatrix();
    cam->updateProjectionMatrix();
    cam->shaderTick();
    OGLGraphics::SetViewportSize((GLint)cam->BottomLeft.x, (GLint)cam->BottomLeft.y, (GLsizei)cam->Width, (GLsizei)cam->Height);
    for (auto& p : mProps) { p->Draw(); }
    for (auto& ap : mAnimProps) {
      InternalShaders::Uber::Get()->SetBool("u_is_animating", false);
      InternalShaders::Stencil::Get()->SetBool("u_is_animating", false);
      if (ap->mAnimator) {
        InternalShaders::Uber::Get()->SetBool("u_is_animating", true);
        InternalShaders::Stencil::Get()->SetBool("u_is_animating", true);
        auto transforms = ap->mAnimator->GetFinalBoneMatrices();
        for (unsigned int i = 0; i < transforms.size(); ++i) {
          InternalShaders::Uber::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
          InternalShaders::Stencil::Get()->SetMat4("u_final_bone_mats[" + std::to_string(i) + "]", transforms[i]);
        }
      }
      ap->Draw();
    }
    if (mSkybox) { mSkybox->Render(cam); }
#ifdef _DEBUG
    if (mSimulateWorldPhysics) { NVidiaPhysx::Get()->DrawDebug(cam); }
#endif
  }

  // render imgui interface
  if (mIMGUI) {
    mIMGUI->NewFrame();
    for (auto& oIU : onImGuiUpdate) { oIU.second(); }
    mIMGUI->Render();
  }

  // display frame changes
  mWindow->swap_buffers();
}

// Runs Once on Engine Shutdown
void Interface::teardown() {
  // run user preferred functions first
  for (auto& oTD : onQuit) {
    oTD.second();
  }

  mCameras.clear();

  ClearAllRuntimeLamdaFunctions();

  InternalShaders::Stencil::Shutdown();
  InternalShaders::Uber::Shutdown();
  InternalShaders::Shadow::Shutdown();

  // delete all the meshes and textures from GPU memory
  for (const auto& p : mProps) {
    p->RemoveCache();
  }
  mProps.clear();

  for (const auto& ap : mAnimProps) {
    ap->RemoveCache();
  }
  mAnimProps.clear();

  for (auto& anim : mAnimation) {
    anim.reset();
  }
  mAnimation.clear();

  mDirectionalLight.reset();
  mPointLights.clear();
  mSpotLights.clear();

  // remove skybox if it exists
  RemoveSkybox();

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
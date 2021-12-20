#include "../include/AncientArcher/AncientArcher.h"
#include "OS/Interface/Window.h"
#include "Physics/NVidiaPhysx.h"
#include "Scene/Camera.h"
#include "Mesh/Prop.h"
#include "Mesh/AnimProp.h"
#include "Scene/Lights.h"
#include "Scene/Skybox.h"
#include "OS/OpenGL/OGLGraphics.h"
#include "Sound/SoundDevice.h"
#include "Sound/Speaker.h"
#include "Sound/SoundEffect.h"
#include "Sound/LongSound.h"
#include "GUI/imGUI.h"
#include "DefaultShaders.h"
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
void AncientArcher::begin() {
  for (const auto& oB : onStart) {
    oB.second();
  }
}

// Runs core and all user defined functionality
void AncientArcher::update() {
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
    for (auto& oKH : onKeyHandling) {
      oKH.second(g_keyboard_input_status);
    }
    g_new_key_reads = false;
  }

  for (auto& oMH : onMouseHandling) { oMH.second(g_mouse_input_status); }
  for (auto& oSH : onScrollHandling) { oSH.second(g_scroll_input_status); }

  // Pre-render
  if (g_os_window_resized) {
    for (auto& cam : mCameras) {
      cam->updateProjectionMatrix(mWindow->GetCurrentWidth(), mWindow->GetCurrentHeight());
    }
    g_os_window_resized = false;
  }

  for (auto& cam : mCameras) {
    cam->shaderTick();
  }

  OGLGraphics::ClearScreen();
  OGLGraphics::SetDepthTest(true);
  OGLGraphics::SetDepthMode(GL_LESS);

}

// Renders visable props every frame
void AncientArcher::render() {
  OGLShader* shader = DefaultShaders::get_ubershader();

  if (!mCameras.empty()) {
    shader->SetBool("isAnimating", false);
    for (auto& p : mProps) {
      p->Draw();
    }
    for (auto& ap : mAnimProps) {
      shader->SetBool("isAnimating", false);
      if (ap->mAnimator) {
        shader->SetBool("isAnimating", true);
        auto transforms = ap->mAnimator->GetFinalBoneMatrices();
        for (unsigned int i = 0; i < transforms.size(); ++i) {
          shader->SetMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
        }
      }
      ap->Draw();
    }
    if (mSkybox) {
      mSkybox->Render(mCameras.front());
    }

    OGLGraphics::SetDepthTest(false);

#ifdef _DEBUG
    if (mSimulateWorldPhysics) {
      NVidiaPhysx::Get()->DrawDebug(mCameras.front());
    }
#endif

  }  // endif !mCamera.empty()
#ifdef _DEBUG
  else {
    std::cout << "0 cameras, skybox and props wont show\n";
  }
#endif

  if (mIMGUI) {
    mIMGUI->NewFrame();
    for (auto& oIU : onImGuiUpdate) {
      oIU.second();
    }
    mIMGUI->Render();
  }

  mWindow->swap_buffers();
}

// Runs Once on Engine Shutdown
void AncientArcher::teardown() {
  // run user preferred functions first
  for (auto& oTD : onQuit) {
    oTD.second();
  }

  // delete all the meshes and textures from GPU memory
  for (const auto& p : mProps) {
    p->RemoveCache();
  }
  mProps.clear();

  for (const auto& ap : mAnimProps) {
    ap->RemoveCache();
  }
  mAnimProps.clear();

  // delete imgui
  if (mIMGUI) {
    mIMGUI->Shutdown();
  }

  mSpeakers.clear();

  mSoundEffects.clear();

  if (mMusic) {
    RemoveMusic();
  }

  isInit = false;
}
}
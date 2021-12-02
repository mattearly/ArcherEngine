#include "../include/AncientArcher/AncientArcher.h"
#include "../include/AncientArcher/Version.h"
#include "Physics/Physics.h"
#include "Scene/Camera.h"
#include "Mesh/Prop.h"
#include "Mesh/AnimProp.h"
#include "Scene/Lights.h"
#include "Scene/Skybox.h"
#include "Renderer/MeshLoader.h"
#include "Renderer/AnimMeshLoader.h"
#include "Renderer/TextureLoader.h"
#include "Renderer/OpenGL/OGLGraphics.h"
#include "Renderer/OSInterface/Window.h"
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

bool AncientArcher::Init() {
  if (isInit) {
    return false;
  }

  // create a new window with the default options
  // todo (matt): allow custom option init
  mWindow = new Window();
  SetIMGUI(true);

  // start base default shaders
  DefaultShaders::init_ubershader();

  // lazy init sound
  SoundDevice::Init();

  // lazy init physics
  Physics::Get();  // returns a pointer to implementation, ignored here

  isInit = true;
  return true;
}

// Runs the Engine cycle of core routines
// return: 0 on exit success, -4 on engine not initialized, -5 if no window
// can throw in an extension or user function
int AncientArcher::Run() {
  if (!isInit) {
    return -4;  // not init
  }
  if (!mWindow) {
    return -5; // no window init yet on this instance
  }
  begin();
  while (!mWindow->GetShouldClose()) {
    update();
    render();
    g_poll_input_events();
  }
  teardown();
  return 0;
}

// Sets the window to close on the next given chance (probably next frame)
void AncientArcher::Shutdown() noexcept {
  if (mWindow)
    mWindow->Close();
  else
    exit(-1);  // probabably shouldn't do this, but its a rare case anyways
}

// resets all vars but leaves the window alone
void AncientArcher::SoftReset() noexcept {
  teardown();

  mCameras.clear();

  onStart.clear();
  onUpdate.clear();
  onKeyHandling.clear();
  onScrollHandling.clear();
  onMouseHandling.clear();
  onMouseButtonHandling.clear();
  onQuit.clear();

  SetCursorToNormal();
}

// Camera
unsigned int AncientArcher::AddCamera(const int w, const int h) {
  if (mCameras.size() > 0) {
    throw("already has a camera, only one cam supported in this version");
  }

  mCameras.emplace_back(std::move(std::make_shared<Camera>((w < 0) ? 0 : w, (h < 0) ? 0 : h)));
  return mCameras.back()->GetUID();
}

bool AncientArcher::RemoveCamera(const int camId) {
  if (mCameras.empty())
    return false;

  auto before_size = mCameras.size();

  auto ret_it = mCameras.erase(
    std::remove_if(mCameras.begin(), mCameras.end(), [&](auto& c) { return c->GetUID() == camId; }),
    mCameras.end());

  auto after_size = mCameras.size();

  if (before_size != after_size)
    return true;  // success remove

  return false;   // fail remove
}

void AncientArcher::SetCamMaxRenderDistance(int camId, float amt) {
  if (amt == 0.f)
    throw("can't set render distance to 0");

  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      if (amt < 0.f)
        amt = abs(amt);
      cam->MaxRenderDistance = amt;
      cam->updateProjectionMatrix(mWindow->GetCurrentWidth(), mWindow->GetCurrentHeight());
      return;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

void AncientArcher::SetCamToPerspective(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      cam->mProjectionType = ProjectionType::PERSPECTIVE;
      cam->updateProjectionMatrix(mWindow->GetCurrentWidth(), mWindow->GetCurrentHeight());
      return;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

void AncientArcher::SetCamToOrtho_testing(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      cam->mProjectionType = ProjectionType::ORTHO;
      cam->updateProjectionMatrix(mWindow->GetCurrentWidth(), mWindow->GetCurrentHeight());
    }
  }
  throw("cam id doesn't exist or is invalid");
}

void AncientArcher::SetCamFOV(int camId, float new_fov) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      if (cam->mProjectionType != ProjectionType::PERSPECTIVE)
        throw("changing FoV in ortho does nothing");
      if (new_fov < 1.f) new_fov = 1.f;
      if (new_fov > 360.f) new_fov = 360.f;
      cam->FOV = abs(new_fov);
      cam->updateProjectionMatrix(mWindow->GetCurrentWidth(), mWindow->GetCurrentHeight());
      return;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

void AncientArcher::SetCamDimensions_testing(int camId, int w, int h) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      cam->updateProjectionMatrix(mWindow->GetCurrentWidth(), mWindow->GetCurrentHeight());
    }
  }
  throw("cam id doesn't exist or is invalid");
}

void AncientArcher::SetCamPosition(int camId, glm::vec3 new_loc) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      cam->Position = new_loc;
      cam->updateCameraVectors();
      return;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

void AncientArcher::SetCamPitch(int camId, float new_pitch_degrees) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      if (new_pitch_degrees > 89.9f)
        new_pitch_degrees = 89.9f;
      else if (new_pitch_degrees < -89.9f)
        new_pitch_degrees = -89.9f;
      cam->Pitch = new_pitch_degrees;
      cam->updateCameraVectors();
      return;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

void AncientArcher::SetCamYaw(int camId, float new_yaw_degrees) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      if (new_yaw_degrees > 360.0f)
        new_yaw_degrees -= 360.f;
      else if (new_yaw_degrees < 0.f)
        new_yaw_degrees += 360.f;
      cam->Yaw = new_yaw_degrees;
      cam->updateCameraVectors();
      return;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

void AncientArcher::ShiftCamPosition(int camId, glm::vec3 offset) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      cam->Position += offset;
      cam->updateCameraVectors();
      return;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

void AncientArcher::ShiftCamPitchAndYaw(int camId, double pitch_offset_degrees, double yaw_offset_degrees) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      double new_pitch_degrees = cam->Pitch + pitch_offset_degrees;
      if (new_pitch_degrees > 89.9f)
        new_pitch_degrees = 89.9f;
      else if (new_pitch_degrees < -89.9f)
        new_pitch_degrees = -89.9f;
      cam->Pitch = static_cast<float>(new_pitch_degrees);

      double new_yaw_degrees = cam->Yaw + yaw_offset_degrees;
      if (new_yaw_degrees > 360.0f)
        new_yaw_degrees -= 360.f;
      else if (new_yaw_degrees < 0.f)
        new_yaw_degrees += 360.f;
      cam->Yaw = static_cast<float>(new_yaw_degrees);

      cam->updateCameraVectors();
      return;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

glm::vec3 AncientArcher::GetCamFront(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return cam->Front;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

glm::vec3 AncientArcher::GetCamRight(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return cam->Right;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

glm::vec3 AncientArcher::GetCamPosition(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return cam->Position;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

float AncientArcher::GetCamPitch(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return cam->Pitch;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

float AncientArcher::GetCamYaw(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return cam->Yaw;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

glm::vec2 AncientArcher::GetPitchAndYaw(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return glm::vec2(cam->Pitch, cam->Yaw);
    }
  }
  throw("cam id doesn't exist or is invalid");
}

glm::mat4 AncientArcher::GetProjectionMatrix(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return cam->mProjectionMatrix;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

glm::mat4 AncientArcher::GetViewMatrix(int camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return cam->mViewMatrix;
    }
  }
  throw("cam id doesn't exist or is invalid");
}

unsigned int AncientArcher::AddProp(const char* path, glm::vec3 location, glm::vec3 scale) {
  mProps.emplace_back(std::make_shared<Prop>(path));
  mProps.back()->spacial_data.MoveTo(location);
  mProps.back()->spacial_data.ScaleTo(scale);
  return mProps.back()->GetUID();
}

void AncientArcher::MoveProp(const unsigned int id, glm::vec3 loc) {
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      return prop->spacial_data.MoveTo(loc);
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void AncientArcher::ScaleProp(const unsigned int id, glm::vec3 scale) {
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      return prop->spacial_data.ScaleTo(scale);
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void AncientArcher::RotateProp(const unsigned int id, glm::vec3 rot) {
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      return prop->spacial_data.RotateTo(rot);
    }
  }
  throw("prop id doesn't exist or is invalid");
}

unsigned int AncientArcher::AddAnimProp(const char* path, glm::vec3 starting_location) {
  mAnimProps.emplace_back(std::make_shared<AnimProp>(path));
  mAnimProps.back()->spacial_data.MoveTo(starting_location);
#ifdef _DEBUG
  std::cout << "loaded: " << path << ", id: " << mAnimProps.back()->GetUID() << std::endl;
#endif
  return mAnimProps.back()->GetUID();
}

void AncientArcher::MoveAnimProp(const unsigned int id, glm::vec3 loc) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      return prop->spacial_data.MoveTo(loc);
    }
  }
  throw("anim prop id doesn't exist or is invalid");
}

void AncientArcher::ScaleAnimProp(const unsigned int id, glm::vec3 scale) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      return prop->spacial_data.ScaleTo(scale);
    }
  }
  throw("anim prop id doesn't exist or is invalid");
}

void AncientArcher::RotateAnimProp(const unsigned int id, glm::vec3 rot) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      return prop->spacial_data.RotateTo(rot);
    }
  }
  throw("anim prop id doesn't exist or is invalid");
}

unsigned int AncientArcher::GetAnimPropBoneCount_testing(const unsigned int anim_prop_id) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == anim_prop_id)
      return prop->m_Skeleton.m_Bones.size();
  }
  return 0;
}

unsigned int AncientArcher::AddAnimation(const char* path, const unsigned int anim_prop_id) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == anim_prop_id) {
      mAnimation.emplace_back(std::make_shared<Animation>(path, prop));
      return mAnimation.back()->GetUID();
    }
  }
  throw("prop id doesn't exist");
}

bool AncientArcher::RemoveAnimation(const unsigned int animation_id) {
  auto before_size = mAnimation.size();

  auto ret_it = mAnimation.erase(
    std::remove_if(
      mAnimation.begin(),
      mAnimation.end(),
      [&](auto& anim) { return anim->GetUID() == animation_id; }),
    mAnimation.end());

  auto after_size = mAnimation.size();

  // return true if successful removal, false otherwise
  return (before_size != after_size);
}

void AncientArcher::SetAnimationOnAnimProp(const unsigned int animation_id, const unsigned int animprop_id) {
  // this is terribly inefficient, but it should work
  for (auto& animprop : mAnimProps) {
    if (animprop->GetUID() == animprop_id) { // animated prop exists
      if (animation_id == -1) { // -1 means reset
        if (animprop->mAnimator) {
          animprop->mAnimator.reset();
        }
        return;  // done
      } else {  // not trying to reset
        for (auto& animation : mAnimation) {
          if (animation->GetUID() == animation_id) { // animation exists
            animprop->SetAnimator(animation);
            return;
          }
        }
      }
    }
  }
  throw("invalid animation id or animated prop id");
}

void AncientArcher::AddPropPhysics(const int prop_id, const COLLIDERTYPE type) {
  // todo: fix
  for (auto& p : mProps) {
    if (p->GetUID() == prop_id) {
      switch (type) {
      case COLLIDERTYPE::BOX:
        p->mMeshes.front().physicsBody = Physics::Get()->CreateBox(
          physx::PxVec3(p->mMeshes.front().local_transform[3][0], p->mMeshes.front().local_transform[3][1], p->mMeshes.front().local_transform[3][2]),
          physx::PxVec3(1, 1, 1));
        break;
      case COLLIDERTYPE::SPHERE:
        p->mMeshes.front().physicsBody = Physics::Get()->CreateSphere(
          physx::PxVec3(p->mMeshes.front().local_transform[3][0], p->mMeshes.front().local_transform[3][1], p->mMeshes.front().local_transform[3][2]),
          1.f);
        break;
      case COLLIDERTYPE::CAPSULE:
        p->mMeshes.front().physicsBody = Physics::Get()->CreateCapsule(
          physx::PxVec3(p->mMeshes.front().local_transform[3][0], p->mMeshes.front().local_transform[3][1], p->mMeshes.front().local_transform[3][2]),
          1.f,
          2.f);
        break;
      }
      return;
    }
  }
  throw("prop id does not exist");
}

void AncientArcher::AddGroundPlane(const glm::vec3 norm, float distance) {
  Physics::Get()->CreateGroundPlane(physx::PxVec3(norm.x, norm.y, norm.z), distance);
}

void AncientArcher::SimulateWorldPhysics(bool status) {
  mSimulateWorldPhysics = status;
}

void AncientArcher::SetSkybox(std::vector<std::string> incomingSkymapFiles) noexcept {
  if (mSkybox)
    return;  // already set
  if (incomingSkymapFiles.size() != 6)
    return;  // invalid size for a skybox
  mSkybox = new Skybox(incomingSkymapFiles);
}

void AncientArcher::SetDirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) {
  if (!mDirectionalLight) {
    mDirectionalLight = std::make_shared<DirectionalLight>(dir, amb, diff, spec);
  } else {
    mDirectionalLight->Direction = dir;
    mDirectionalLight->Ambient = amb;
    mDirectionalLight->Diffuse = diff;
    mDirectionalLight->Specular = spec;
  }

  {
    assert(DefaultShaders::get_ubershader());
    DefaultShaders::get_ubershader()->Use();
    DefaultShaders::get_ubershader()->SetInt("isDirectionalLightOn", 1);
    DefaultShaders::get_ubershader()->SetVec3("directionalLight.Direction", mDirectionalLight->Direction);
    DefaultShaders::get_ubershader()->SetVec3("directionalLight.Ambient", mDirectionalLight->Ambient);
    DefaultShaders::get_ubershader()->SetVec3("directionalLight.Diffuse", mDirectionalLight->Diffuse);
    DefaultShaders::get_ubershader()->SetVec3("directionalLight.Specular", mDirectionalLight->Specular);
  }
}

void AncientArcher::RemoveDirectionalLight() {
  assert(DefaultShaders::get_ubershader());
  DefaultShaders::get_ubershader()->Use();
  DefaultShaders::get_ubershader()->SetInt("isDirectionalLightOn", 0);
  mDirectionalLight.reset();
}

// Point Light
int AncientArcher::AddPointLight(glm::vec3 pos, float constant, float linear, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) {
  if (mPointLights.size() >= MAXPOINTLIGHTS)
    throw("too many point lights");

  mPointLights.emplace_back(std::move(std::make_shared<PointLight>(pos, constant, linear, quad, amb, diff, spec)));
  std::size_t new_point_size = mPointLights.size() - 1;

  // push changes to shader
  {
    std::string position, constant, linear, quadratic, ambient, diffuse, specular;
    constant = linear = quadratic = ambient = diffuse = specular = position = "pointLight[";
    std::stringstream ss;
    ss << new_point_size;
    position += ss.str();
    constant += ss.str();
    linear += ss.str();
    quadratic += ss.str();
    ambient += ss.str();
    diffuse += ss.str();
    specular += ss.str();
    position += "].";
    constant += "].";
    linear += "].";
    quadratic += "].";
    ambient += "].";
    diffuse += "].";
    specular += "].";
    position += "Position";
    constant += "Constant";
    linear += "Linear";
    quadratic += "Quadratic";
    ambient += "Ambient";
    diffuse += "Diffuse";
    specular += "Specular";

    assert(DefaultShaders::get_ubershader());
    DefaultShaders::get_ubershader()->Use();
    DefaultShaders::get_ubershader()->SetVec3(position, mPointLights.back()->Position);
    DefaultShaders::get_ubershader()->SetFloat(constant, mPointLights.back()->Constant);
    DefaultShaders::get_ubershader()->SetFloat(linear, mPointLights.back()->Linear);
    DefaultShaders::get_ubershader()->SetFloat(quadratic, mPointLights.back()->Quadratic);
    DefaultShaders::get_ubershader()->SetVec3(ambient, mPointLights.back()->Ambient);
    DefaultShaders::get_ubershader()->SetVec3(diffuse, mPointLights.back()->Diffuse);
    DefaultShaders::get_ubershader()->SetVec3(specular, mPointLights.back()->Specular);
    DefaultShaders::get_ubershader()->SetInt("NUM_POINT_LIGHTS", static_cast<int>(new_point_size + 1));
  }
  return mPointLights.back()->id;  // unique id
}

bool AncientArcher::RemovePointLight(int which_by_id) {
  // returns true if successfully removed the point light, false otherwise
  if (mPointLights.empty())
    return false;

  auto before_size = mPointLights.size();

  auto ret_it = mPointLights.erase(
    std::remove_if(mPointLights.begin(), mPointLights.end(), [&](auto& sl) { return sl->id == which_by_id; }),
    mPointLights.end());

  auto after_size = mPointLights.size();

  if (before_size != after_size) {
    DefaultShaders::get_ubershader()->Use();
    DefaultShaders::get_ubershader()->SetInt("NUM_POINT_LIGHTS", static_cast<int>(after_size));

    // sync lights on shader after the change
    for (int i = 0; i < after_size; i++) {
      ChangePointLight(
        mPointLights[i]->id,
        mPointLights[i]->Position,
        mPointLights[i]->Constant,
        mPointLights[i]->Linear,
        mPointLights[i]->Quadratic,
        mPointLights[i]->Ambient,
        mPointLights[i]->Diffuse,
        mPointLights[i]->Specular
      );
    }
    return true;
  } else
    return false;
}

void AncientArcher::MovePointLight(int which, glm::vec3 new_pos) {
  if (which < 0)
    throw("dont");

  int loc_in_vec = 0;
  for (auto& pl : mPointLights) {
    if (pl->id == which) {
      pl->Position = new_pos;
      std::stringstream ss;
      ss << loc_in_vec;
      std::string position = "pointLight[" + ss.str() + "].Position";
      DefaultShaders::get_ubershader()->Use();
      DefaultShaders::get_ubershader()->SetVec3(position.c_str(), pl->Position);
      return;
    }
    loc_in_vec++;
  }
  throw("u messed up");
}

void AncientArcher::ChangePointLight(int which, glm::vec3 new_pos, float new_constant, float new_linear, float new_quad,
  glm::vec3 new_amb, glm::vec3 new_diff, glm::vec3 new_spec) {
  if (which < 0)
    throw("dont");

  int loc_in_vec = 0;
  for (auto& pl : mPointLights) {
    if (pl->id == which) {
      // push changes to shader
      {
        pl->Position = new_pos;
        pl->Ambient = new_amb;
        pl->Constant = new_constant;
        pl->Diffuse = new_diff;
        pl->Linear = new_linear;
        pl->Quadratic = new_quad;
        pl->Specular = new_spec;
        std::string pos, ambient, constant, diffuse, linear, quadrat, specular;
        ambient = constant = diffuse = linear = quadrat = specular = pos = "pointLight[";
        std::stringstream ss;
        ss << loc_in_vec;
        pos += ss.str();
        constant += ss.str();
        linear += ss.str();
        quadrat += ss.str();
        ambient += ss.str();
        diffuse += ss.str();
        specular += ss.str();
        pos += "].";
        constant += "].";
        linear += "].";
        quadrat += "].";
        ambient += "].";
        diffuse += "].";
        specular += "].";
        pos += "Position";
        constant += "Constant";
        linear += "Linear";
        quadrat += "Quadratic";
        ambient += "Ambient";
        diffuse += "Diffuse";
        specular += "Specular";

        DefaultShaders::get_ubershader()->Use();
        DefaultShaders::get_ubershader()->SetVec3(pos, pl->Position);
        DefaultShaders::get_ubershader()->SetFloat(constant, pl->Constant);
        DefaultShaders::get_ubershader()->SetFloat(linear, pl->Linear);
        DefaultShaders::get_ubershader()->SetFloat(quadrat, pl->Quadratic);
        DefaultShaders::get_ubershader()->SetVec3(ambient, pl->Ambient);
        DefaultShaders::get_ubershader()->SetVec3(diffuse, pl->Diffuse);
        DefaultShaders::get_ubershader()->SetVec3(specular, pl->Specular);
      }
      return;
    }
    loc_in_vec++;
  }
  throw("u messed up");
}

int AncientArcher::AddSpotLight(
  glm::vec3 pos, glm::vec3 dir, float inner, float outer, float constant, float linear, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) {
  if (mSpotLights.size() == MAXSPOTLIGHTS) {
    throw("too many spot lights");
  }

  mSpotLights.emplace_back(std::move(std::make_shared<SpotLight>(pos, dir, inner, outer, constant, linear, quad, amb, diff, spec)));
  auto new_spot_loc = mSpotLights.size() - 1;

  // push changes to shader
  {
    std::string pos, ambient, constant, cutoff, ocutoff, diffuse, direction, linear, quadrat, specular;
    ambient = constant = cutoff = ocutoff = diffuse = direction = linear = quadrat = specular = pos = "spotLight[";
    std::stringstream ss;
    ss << new_spot_loc;
    pos += ss.str();
    constant += ss.str();
    cutoff += ss.str();
    ocutoff += ss.str();
    direction += ss.str();
    linear += ss.str();
    quadrat += ss.str();
    ambient += ss.str();
    diffuse += ss.str();
    specular += ss.str();
    pos += "].";
    constant += "].";
    cutoff += "].";
    ocutoff += "].";
    direction += "].";
    linear += "].";
    quadrat += "].";
    ambient += "].";
    diffuse += "].";
    specular += "].";
    pos += "Position";
    constant += "Constant";
    cutoff += "CutOff";
    ocutoff += "OuterCutOff";
    direction += "Direction";
    linear += "Linear";
    quadrat += "Quadratic";
    ambient += "Ambient";
    diffuse += "Diffuse";
    specular += "Specular";

    assert(DefaultShaders::get_ubershader());
    DefaultShaders::get_ubershader()->Use();
    DefaultShaders::get_ubershader()->SetVec3(pos, mSpotLights.back()->Position);
    DefaultShaders::get_ubershader()->SetFloat(cutoff, mSpotLights.back()->CutOff);
    DefaultShaders::get_ubershader()->SetFloat(ocutoff, mSpotLights.back()->OuterCutOff);
    DefaultShaders::get_ubershader()->SetVec3(direction, mSpotLights.back()->Direction);
    DefaultShaders::get_ubershader()->SetFloat(constant, mSpotLights.back()->Constant);
    DefaultShaders::get_ubershader()->SetFloat(linear, mSpotLights.back()->Linear);
    DefaultShaders::get_ubershader()->SetFloat(quadrat, mSpotLights.back()->Quadratic);
    DefaultShaders::get_ubershader()->SetVec3(ambient, mSpotLights.back()->Ambient);
    DefaultShaders::get_ubershader()->SetVec3(diffuse, mSpotLights.back()->Diffuse);
    DefaultShaders::get_ubershader()->SetVec3(specular, mSpotLights.back()->Specular);
    DefaultShaders::get_ubershader()->SetInt("NUM_SPOT_LIGHTS", static_cast<int>(new_spot_loc + 1));
  }
  return mSpotLights.back()->id;  // unique id
}

bool AncientArcher::RemoveSpotLight(int which_by_id) {
  // returns true if it removed the spot light, false otherwise
  if (mSpotLights.empty())
    return false;

  auto before_size = mSpotLights.size();

  /*auto ret_it = */mSpotLights.erase(
    std::remove_if(mSpotLights.begin(), mSpotLights.end(), [&](auto& sl) { return sl->id == which_by_id; }),
    mSpotLights.end());

  auto after_size = mSpotLights.size();

  if (before_size != after_size) {
    DefaultShaders::get_ubershader()->Use();
    DefaultShaders::get_ubershader()->SetInt("NUM_SPOT_LIGHTS", static_cast<int>(after_size));

    // sync lights on shader after the change
    for (int i = 0; i < after_size; i++) {
      ChangeSpotLight(
        mSpotLights[i]->id,
        mSpotLights[i]->Position,
        mSpotLights[i]->Direction,
        mSpotLights[i]->CutOff,
        mSpotLights[i]->OuterCutOff,
        mSpotLights[i]->Constant,
        mSpotLights[i]->Linear,
        mSpotLights[i]->Quadratic,
        mSpotLights[i]->Ambient,
        mSpotLights[i]->Diffuse,
        mSpotLights[i]->Specular
      );
    }
    return true;
  } else
    return false;
}

void AncientArcher::MoveSpotLight(int which, glm::vec3 new_pos, glm::vec3 new_dir) {
  if (which < 0)
    throw("dont");

  int loc_in_vec = 0;
  for (auto& sl : mSpotLights) {
    if (sl->id == which) {
      sl->Position = new_pos;
      sl->Direction = new_dir;
      DefaultShaders::get_ubershader()->Use();
      std::stringstream ss;
      ss << loc_in_vec;
      std::string position = "spotLight[" + ss.str() + "].Position";
      std::string direction = "spotLight[" + ss.str() + "].Direction";
      DefaultShaders::get_ubershader()->SetVec3(position, sl->Position);
      DefaultShaders::get_ubershader()->SetVec3(direction, sl->Direction);
      return;
    }
    loc_in_vec++;
  }
  throw("u messed up");
}

void AncientArcher::ChangeSpotLight(int which, glm::vec3 new_pos, glm::vec3 new_dir, float new_inner,
  float new_outer, float new_constant, float new_linear, float new_quad, glm::vec3 new_amb,
  glm::vec3 new_diff, glm::vec3 new_spec) {
  if (which < 0)
    throw("dont");

  int loc_in_vec = 0;
  for (auto& sl : mSpotLights) {
    if (sl->id == which) {
      // push changes to shader
      {
        sl->Position = new_pos;
        sl->Direction = new_dir;
        sl->Ambient = new_amb;
        sl->Constant = new_constant;
        sl->CutOff = new_inner;
        sl->OuterCutOff = new_outer;
        sl->Diffuse = new_diff;
        sl->Linear = new_linear;
        sl->Quadratic = new_quad;
        sl->Specular = new_spec;
        std::string pos, ambient, constant, cutoff, ocutoff, diffuse, direction, linear, quadrat, specular;
        ambient = constant = cutoff = ocutoff = diffuse = direction = linear = quadrat = specular = pos = "spotLight[";
        std::stringstream ss;
        ss << loc_in_vec;
        pos += ss.str();
        constant += ss.str();
        cutoff += ss.str();
        ocutoff += ss.str();
        direction += ss.str();
        linear += ss.str();
        quadrat += ss.str();
        ambient += ss.str();
        diffuse += ss.str();
        specular += ss.str();
        pos += "].";
        constant += "].";
        cutoff += "].";
        ocutoff += "].";
        direction += "].";
        linear += "].";
        quadrat += "].";
        ambient += "].";
        diffuse += "].";
        specular += "].";
        pos += "Position";
        constant += "Constant";
        cutoff += "CutOff";
        ocutoff += "OuterCutOff";
        direction += "Direction";
        linear += "Linear";
        quadrat += "Quadratic";
        ambient += "Ambient";
        diffuse += "Diffuse";
        specular += "Specular";

        DefaultShaders::get_ubershader()->Use();
        DefaultShaders::get_ubershader()->SetVec3(pos, sl->Position);
        DefaultShaders::get_ubershader()->SetFloat(cutoff, sl->CutOff);
        DefaultShaders::get_ubershader()->SetFloat(ocutoff, sl->OuterCutOff);
        DefaultShaders::get_ubershader()->SetVec3(direction, sl->Direction);
        DefaultShaders::get_ubershader()->SetFloat(constant, sl->Constant);
        DefaultShaders::get_ubershader()->SetFloat(linear, sl->Linear);
        DefaultShaders::get_ubershader()->SetFloat(quadrat, sl->Quadratic);
        DefaultShaders::get_ubershader()->SetVec3(ambient, sl->Ambient);
        DefaultShaders::get_ubershader()->SetVec3(diffuse, sl->Diffuse);
        DefaultShaders::get_ubershader()->SetVec3(specular, sl->Specular);
      }
      return;
    }
    loc_in_vec++;
  }
  throw("u messed up");
}

// Sound Effects
unsigned int AncientArcher::AddSoundEffect(const char* path) {
  // make sure the sound effect hasn't already been loaded
  for (const auto& pl : mSoundEffects) {
    if (path == pl->_FilePath.c_str())
      throw("sound from that path already loaded");
  }
  // Add a new sound effect
  mSoundEffects.reserve(mSoundEffects.size() + 1);
  mSoundEffects.push_back(std::move(std::make_shared<SoundEffect>(path)));

  // Add a new speaker
  mSpeakers.reserve(mSpeakers.size() + 1);
  mSpeakers.emplace_back(std::move(std::make_shared<Speaker>()));  //test, note, todo: same sound effects can be applied to multiple speakers

  // Associate the sound effect to the speaker.
  mSpeakers.back()->AssociateBuffer(mSoundEffects.back()->_Buffer);

  // return the unique ID of the speaker
  return mSpeakers.back()->GetUID();
}

void AncientArcher::SetSoundEffectVolume(int sound_id, float new_vol) {
  for (auto& se : mSpeakers) {
    if (se == NULL)
      continue;
    if (sound_id == se->GetUID()) {
      se->SetVolume(new_vol);
      return;
    }
  }

  throw("sound effect volume speaker id not found");
}

void AncientArcher::RemoveSoundEffect(int soundId) {
  if (mSoundEffects.empty())
    throw("no sounds exist, nothing to remove");

  auto before_size = mSoundEffects.size();

  auto after_size = mSoundEffects.size();

  if (before_size == after_size)
    throw("didn't remove anything");
}

void AncientArcher::PlaySoundEffect(int id, bool interrupt) {
  if (mSpeakers.empty())
    throw("no speakers");

  for (auto& spkr : mSpeakers) {
    if (spkr->GetUID() == id) {
      if (interrupt) {
        spkr->PlayInterrupt();
        return;
      }
      spkr->PlayNoOverlap();
      return;
    }
  }
  throw("speaker not found");
}

void AncientArcher::AddMusic(const char* path) {
  if (!mMusic) {
    mMusic = new LongSound(path);
    return;
  }
  throw("music already loaded, use remove music first");
}

void AncientArcher::RemoveMusic() {
  if (mMusic) {
    delete mMusic;
    mMusic = NULL;
    return;
  }
  throw("no music to remove");
}

void AncientArcher::PlayMusic() {
  if (mMusic) {
    mMusic->Play();
    return;
  }
  throw("no music to play");
}

void AncientArcher::PauseMusic() {
  if (mMusic) {
    mMusic->Pause();
    return;
  }
  throw("no music loaded");
}

void AncientArcher::ResumeMusic() {
  if (mMusic) {
    mMusic->Resume();
    return;
  }
  throw("no music loaded");
}

void AncientArcher::StopMusic() {
  if (mMusic) {
    mMusic->Stop();
    return;
  }
  throw("no music loaded");
}
void AncientArcher::SetMusicVolume(float new_vol) {
  if (mMusic) {
    mMusic->SetVolume(new_vol);
    return;
  }
  throw("no music loaded");
}

void AncientArcher::SetCursorToHidden() noexcept {
  if (mWindow)
    mWindow->SetCursorToHidden();
}

void AncientArcher::SetCursorToDisabled() noexcept {
  if (mWindow)
    mWindow->SetCursorToDisabled();
}

void AncientArcher::SetCursorToNormal() noexcept {
  if (mWindow)
    mWindow->SetCursorToNormal();
}

void AncientArcher::SetIMGUI(const bool value) {
  if (!value) {
    if (mIMGUI) {
      delete mIMGUI;
      mIMGUI = NULL;
    }
  } else {
    if (mIMGUI) {
      return;
    } else {
      mIMGUI = new imGUI();
      mIMGUI->InitOpenGL(mWindow->mGLFWwindow);
    }
  }
}

void AncientArcher::SetWindowClearColor(glm::vec3 color) noexcept {
  OGLGraphics::SetViewportClearColor(color);
}

// returns -1 if there is no window
int AncientArcher::GetWindowWidth() noexcept {
  if (!mWindow) return -1;
  return mWindow->GetCurrentWidth();
}

// returns -1 if there is no window
int AncientArcher::GetWindowHeight() noexcept {
  if (!mWindow) return -1;
  return mWindow->GetCurrentHeight();
}

// changes the window title, does nothing if window is null
void AncientArcher::SetWindowTitle(const char* name) noexcept {
  if (!mWindow) return;
  // todo: improve efficiency
  auto temp = mWindow->GetModifiableWindowOptions();
  temp->_title = name;
  mWindow->ApplyChanges();
}

// toggles fullscreen as expected, does nothign if window is null
void AncientArcher::ToggleFullscreen(bool try_borderless) noexcept {
  if (!mWindow) return;
  // todo: improve efficiency
  auto temp = mWindow->GetModifiableWindowOptions();
  if (temp->_windowing_mode == WINDOW_MODE::WINDOWED) {
    if (try_borderless) {
      temp->_windowing_mode = WINDOW_MODE::FULLSCREEN_BORDERLESS;
    } else {
      temp->_windowing_mode = WINDOW_MODE::FULLSCREEN;
    }
  } else {
    temp->_windowing_mode = WINDOW_MODE::WINDOWED;
  }
  mWindow->ApplyChanges();
}

unsigned int AncientArcher::AddToOnBegin(void(*function)()) {
  static unsigned int next_begin_id = 0;
  next_begin_id++;
  //onStart.emplace(next_begin_id, function);
  onStart.insert(std::make_pair(next_begin_id, function));
  return next_begin_id;
}

unsigned int AncientArcher::AddToUpdate(void(*function)(float)) {
  static unsigned int next_deltaupdate_id = 0;
  next_deltaupdate_id++;
  onUpdate.emplace(next_deltaupdate_id, function);
  return next_deltaupdate_id;
}

unsigned int AncientArcher::AddToImGuiUpdate(void(*function)()) {
  static unsigned int next_imgui_id = 0;
  next_imgui_id++;
  onImGuiUpdate.emplace(next_imgui_id, function);
  return next_imgui_id;
}

unsigned int AncientArcher::AddToScrollHandling(void(*function)(MouseScrollWheel&)) {
  static unsigned int next_scrollhandling_id = 0;
  next_scrollhandling_id++;
  onScrollHandling.emplace(next_scrollhandling_id, function);
  return next_scrollhandling_id;
}

unsigned int AncientArcher::AddToKeyHandling(void(*function)(KeyboardButtons&)) {
  static unsigned int next_keyhandling_id = 0;
  next_keyhandling_id++;
  onKeyHandling.emplace(next_keyhandling_id, function);
  return next_keyhandling_id;
}

unsigned int AncientArcher::AddToMouseHandling(void(*function)(MouseCursorPos&)) {
  static unsigned int next_mousehandling_id = 0;
  next_mousehandling_id++;
  onMouseHandling.emplace(next_mousehandling_id, function);
  return next_mousehandling_id;
}

unsigned int AncientArcher::AddToMouseButtonHandling(void(*function)(MouseButtons&)) {
  static unsigned int next_mouseButtonhandling_id = 0;
  next_mouseButtonhandling_id++;
  onMouseButtonHandling.emplace(next_mouseButtonhandling_id, function);
  return next_mouseButtonhandling_id;
}

unsigned int AncientArcher::AddToOnTeardown(void(*function)()) {
  static unsigned int next_teardown_id = 0;
  next_teardown_id++;
  onQuit.emplace(next_teardown_id, function);
  return next_teardown_id;
}

bool AncientArcher::RemoveFromOnBegin(unsigned int r_id) {
  return static_cast<bool>(onStart.erase(r_id));
}

bool AncientArcher::RemoveFromUpdate(unsigned int r_id) {
  return static_cast<bool>(onUpdate.erase(r_id));
}

bool AncientArcher::RemoveFromImGuiUpdate(unsigned int r_id) {
  return static_cast<bool>(onImGuiUpdate.erase(r_id));
}

bool AncientArcher::RemoveFromScrollHandling(unsigned int r_id) {
  return static_cast<bool>(onScrollHandling.erase(r_id));
}

bool AncientArcher::RemoveFromKeyHandling(unsigned int r_id) {
  return static_cast<bool>(onKeyHandling.erase(r_id));
}

bool AncientArcher::RemoveFromMouseHandling(unsigned int r_id) {
  return static_cast<bool>(onMouseHandling.erase(r_id));
}

bool AncientArcher::RemoveFromMouseButtonHandling(unsigned int r_id) {
  return static_cast<bool>(onMouseButtonHandling.erase(r_id));
}

bool AncientArcher::RemoveFromTeardown(unsigned int r_id) {
  return static_cast<bool>(onQuit.erase(r_id));
}

/*
*
*
*   <<<< PRIVATE IMPLEMENTATIONS BELOW >>>
*
*
*/

// runs once after a new run start
void AncientArcher::begin() {
  for (const auto& oB : onStart) {
    oB.second();
  }
}

// Runs core and all user defined functionality
void AncientArcher::update() {
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
  for (auto& p : mProps) {
    if (p->spacial_data.modified) {
      p->spacial_data.ProcessModifications();
    }
  }
  for (auto& ap : mAnimProps) {
    if (ap->spacial_data.modified) {
      ap->spacial_data.ProcessModifications();
    }
    if (ap->mAnimator) {
      ap->UpdateAnim(elapsedTime);
    }
  }

  if (mSimulateWorldPhysics) {
    Physics::Get()->StepPhysics(elapsedTime);
  }

  if (mMusic) {
    static float music_rebuffer_cd = 0;
    music_rebuffer_cd += elapsedTime;
    if (music_rebuffer_cd > .5f) {  // todo(maybe): math with file size and stuff to figure out how long this cd should actually be
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
}

// Renders visable props every frame
void AncientArcher::render() {
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

  mWindow->clear_screen();

  OGLGraphics::SetDepthTest(true);
  OGLGraphics::SetDepthMode(GL_LESS);

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
  }
#ifdef _DEBUG
  else {
    std::cout << "0 cameras, skybox and props wont show\n";
  }
#endif

  OGLGraphics::SetDepthTest(false);

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
    MeshLoader::UnloadGameObject(p->mMeshes);  // todo: consider moving to the destructor the prop
  }
  mProps.clear();

  for (const auto& ap : mAnimProps) {
    AnimMeshLoader::UnloadGameObject(ap->mMeshes);
  }
  mAnimProps.clear();

  // delete imgui
  if (mIMGUI) {
    mIMGUI->Shutdown();
  }

  // delete sound
  //for (auto& spkr : mSpeakers) {
  //  spkr.reset();
  //}

  mSpeakers.clear();

  //for (auto& se : mSoundEffects) {
  //  se.reset();
  //}

  mSoundEffects.clear();

  if (mMusic) {
    delete mMusic;
    mMusic = nullptr;
  }

  isInit = false;
}

} // end namespace AA

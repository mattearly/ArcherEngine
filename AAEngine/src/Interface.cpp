#include "../include/AAEngine/Interface.h"
#include "Physics/NVidiaPhysx.h"
#include "../include/AAEngine/Scene/Camera.h"
#include "Mesh/Prop.h"
#include "Mesh/AnimProp.h"
#include "Scene/Lights.h"
#include "Scene/Skybox.h"
#include "OS/OpenGL/OGLGraphics.h"
#include "OS/Interface/Window.h"
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

bool Interface::Init() {
  if (isInit)
    return false;
  mWindow = new Window();
  SetIMGUI(true);
  SoundDevice::Init();
  auto physics_impl = NVidiaPhysx::Get();  // returns a pointer to implementation, ignored here
  isInit = true;
  return true;
}

bool Interface::Init(const WindowOptions& winopts) {
  if (isInit)
    return false;
  mWindow = new Window(winopts);
  SetIMGUI(true);
  SoundDevice::Init();
  auto physics_impl = NVidiaPhysx::Get();  // returns a pointer to implementation, ignored here
  isInit = true;
  return true;
}

bool Interface::Init(std::shared_ptr<WindowOptions> winopts) {
  if (isInit)
    return false;
  mWindow = new Window(winopts);
  SetIMGUI(true);
  SoundDevice::Init();
  auto physics_impl = NVidiaPhysx::Get();  // returns a pointer to implementation, ignored here
  isInit = true;
  return true;
}

// Runs the Engine cycle of core routines
// return: 0 on exit success, -4 on engine not initialized, -5 if no window
// can throw in an extension or user function
int Interface::Run() {
  if (!isInit) {
    return -4;  // not init
  }
  if (!mWindow) {
    return -5;  // no window init yet on this instance
  }
  begin();
  while (!mWindow->GetShouldClose()) {
    update();
    render();
  }
  teardown();
  return 0;
}

// Sets the window to close on the next given chance (probably next frame)
void Interface::Shutdown() noexcept {
  if (mWindow)
    mWindow->Close();
  else
    exit(-1);  // probabably shouldn't do this, but its a rare case anyways
}

// resets all vars but leaves the window alone
void Interface::SoftReset() noexcept {
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
unsigned int Interface::AddCamera(const int w, const int h) {
  if (mCameras.size() > 0) {
    throw("already has a camera, only one cam supported in this version");
  }

  mCameras.emplace_back(std::move(std::make_shared<Camera>((w < 0) ? 0 : w, (h < 0) ? 0 : h)));
  return mCameras.back()->GetUID();
}

bool Interface::RemoveCamera(const int camId) {
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

std::shared_ptr<Camera> Interface::GetCamera(uidtype camId)
{
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return cam;
    }
  }

  throw("cam id doesn't exist or is invalid");
}

unsigned int Interface::AddProp(const char* path, glm::vec3 location, glm::vec3 scale) {
  mProps.emplace_back(std::make_shared<Prop>(path));
  mProps.back()->spacial_data.MoveTo(location);
  mProps.back()->spacial_data.ScaleTo(scale);
  return mProps.back()->GetUID();
}

bool Interface::RemoveProp(const unsigned int id) {

  // remove cache (or decrement count of loaded in when multiloading)
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      prop->RemoveCache();
    }
  }

  // the actual remove
  auto before_size = mProps.size();

  auto ret_it = mProps.erase(
    std::remove_if(
      mProps.begin(),
      mProps.end(),
      [&](auto& prop) { return prop->GetUID() == id; }),
    mProps.end());

  auto after_size = mProps.size();

  // return true if successful removal, false otherwise
  return (before_size != after_size);
}

void Interface::MoveProp(const unsigned int id, glm::vec3 loc) {
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      prop->spacial_data.MoveTo(loc);
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void Interface::ScaleProp(const unsigned int id, glm::vec3 scale) {
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      prop->spacial_data.ScaleTo(scale);
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void Interface::RotateProp(const unsigned int id, glm::vec3 rot) {
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      prop->spacial_data.RotateTo(rot);
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void Interface::StencilProp(const unsigned int id, const bool tf) {
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      prop->stenciled = tf;
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void Interface::StencilPropColor(const unsigned int id, const glm::vec3 color)
{
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      prop->stencil_color = color;
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void Interface::StencilPropWithNormals(const unsigned int id, const bool tf)
{
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      prop->stenciled_with_normals = tf;
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void Interface::StencilPropScale(const unsigned int id, const float scale)
{
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      prop->stencil_scale = scale;
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

unsigned int Interface::AddAnimProp(const char* path, glm::vec3 starting_location) {
  mAnimProps.emplace_back(std::make_shared<AnimProp>(path));
  mAnimProps.back()->spacial_data.MoveTo(starting_location);
#ifdef _DEBUG
  std::cout << "loaded: " << path << ", id: " << mAnimProps.back()->GetUID() << std::endl;
#endif
  return mAnimProps.back()->GetUID();
}

void Interface::MoveAnimProp(const unsigned int id, glm::vec3 loc) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      return prop->spacial_data.MoveTo(loc);
    }
  }
  throw("anim prop id doesn't exist or is invalid");
}

void Interface::ScaleAnimProp(const unsigned int id, glm::vec3 scale) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      return prop->spacial_data.ScaleTo(scale);
    }
  }
  throw("anim prop id doesn't exist or is invalid");
}

void Interface::RotateAnimProp(const unsigned int id, glm::vec3 rot) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      return prop->spacial_data.RotateTo(rot);
    }
  }
  throw("anim prop id doesn't exist or is invalid");
}

void Interface::StencilAnimProp(const unsigned int id, const bool tf)
{
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      prop->stenciled = tf;
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void Interface::StencilAnimPropColor(const unsigned int id, const glm::vec3 color)
{
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      prop->stencil_color = color;
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void Interface::StencilAnimPropWithNormals(const unsigned int id, const bool tf)
{
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      prop->stenciled_with_normals = tf;
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

void Interface::StencilAnimPropScale(const unsigned int id, const float scale)
{
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == id) {
      prop->stencil_scale = scale;
      return;
    }
  }
  throw("prop id doesn't exist or is invalid");
}

unsigned int Interface::GetAnimPropBoneCount_testing(const unsigned int anim_prop_id) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == anim_prop_id)
      return (unsigned int)prop->m_Skeleton.m_Bones.size();
  }
  return 0;
}

unsigned int Interface::AddAnimation(const char* path, const unsigned int anim_prop_id) {
  for (auto& prop : mAnimProps) {
    if (prop->GetUID() == anim_prop_id) {
      mAnimation.emplace_back(std::make_shared<Animation>(path, prop));
      return mAnimation.back()->GetUID();
    }
  }
  throw("prop id doesn't exist");
}

bool Interface::RemoveAnimation(const unsigned int animation_id) {
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

void Interface::SetAnimationOnAnimProp(const unsigned int animation_id, const unsigned int animprop_id) {
  // this is terribly inefficient, but it should work
  for (auto& animprop : mAnimProps) {
    if (animprop->GetUID() == animprop_id) { // animated prop exists
      if (animation_id == -1) { // -1 means reset
        if (animprop->mAnimator) {
          animprop->mAnimator.reset();
        }
        return;  // done
      }
      else {  // not trying to reset
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

void Interface::AddPropPhysics(const int prop_id, const COLLIDERTYPE type) {
  // todo: fix
  for (auto& p : mProps) {
    if (p->GetUID() == prop_id) {
      switch (type) {
      case COLLIDERTYPE::BOX:
        p->spacial_data.mRigidBody =
          NVidiaPhysx::Get()->CreateBox(
            physx::PxVec3(
              p->spacial_data.mFinalModelMatrix[3][0],
              p->spacial_data.mFinalModelMatrix[3][1],
              p->spacial_data.mFinalModelMatrix[3][2]),
            physx::PxVec3(1, 1, 1));
        break;
      case COLLIDERTYPE::SPHERE:
        p->spacial_data.mRigidBody =
          NVidiaPhysx::Get()->CreateSphere(
            physx::PxVec3(
              p->spacial_data.mFinalModelMatrix[3][0],
              p->spacial_data.mFinalModelMatrix[3][1],
              p->spacial_data.mFinalModelMatrix[3][2]),
            1.f);
        break;
      case COLLIDERTYPE::CAPSULE:
        p->spacial_data.mRigidBody =
          NVidiaPhysx::Get()->CreateCapsule(
            physx::PxVec3(
              p->spacial_data.mFinalModelMatrix[3][0],
              p->spacial_data.mFinalModelMatrix[3][1],
              p->spacial_data.mFinalModelMatrix[3][2]),
            1.f,
            2.f);
        break;
      }
      return;
    }
  }
  throw("prop id does not exist");
}

void Interface::AddGroundPlane(const glm::vec3 norm, float distance) {
  NVidiaPhysx::Get()->CreateGroundPlane(physx::PxVec3(norm.x, norm.y, norm.z), distance);
}

void Interface::SimulateWorldPhysics(bool status) {
  mSimulateWorldPhysics = status;
}

void Interface::SetSkybox(std::vector<std::string> incomingSkymapFiles) noexcept {
  if (mSkybox)
    return;  // already set
  if (incomingSkymapFiles.size() != 6)
    return;  // invalid size for a skybox
  mSkybox = std::make_shared<Skybox>(incomingSkymapFiles);
}

void Interface::RemoveSkybox() noexcept {
  if (mSkybox)
    mSkybox.reset();
}

void Interface::SetDirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) {
  if (!mDirectionalLight) {
    mDirectionalLight = std::make_shared<DirectionalLight>(dir, amb, diff, spec);
  }
  else {
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

void Interface::RemoveDirectionalLight() {
  assert(DefaultShaders::get_ubershader());
  DefaultShaders::get_ubershader()->Use();
  DefaultShaders::get_ubershader()->SetInt("isDirectionalLightOn", 0);
  mDirectionalLight.reset();
}

// Point Light
int Interface::AddPointLight(glm::vec3 pos, float constant, float linear, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) {
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

bool Interface::RemovePointLight(int which_by_id) {
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
  }
  else
    return false;
}

void Interface::MovePointLight(int which, glm::vec3 new_pos) {
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

void Interface::ChangePointLight(int which, glm::vec3 new_pos, float new_constant, float new_linear, float new_quad,
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

int Interface::AddSpotLight(
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

bool Interface::RemoveSpotLight(int which_by_id) {
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
  }
  else
    return false;
}

void Interface::MoveSpotLight(int which, glm::vec3 new_pos, glm::vec3 new_dir) {
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

void Interface::ChangeSpotLight(int which, glm::vec3 new_pos, glm::vec3 new_dir, float new_inner,
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
unsigned int Interface::AddSoundEffect(const char* path) {
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

void Interface::SetSoundEffectVolume(int sound_id, float new_vol) {
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

void Interface::RemoveSoundEffect(int soundId) {
  if (mSoundEffects.empty())
    throw("no sounds exist, nothing to remove");

  auto before_size = mSoundEffects.size();

  auto after_size = mSoundEffects.size();

  if (before_size == after_size)
    throw("didn't remove anything");
}

void Interface::PlaySoundEffect(int id, bool interrupt) {
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

void Interface::AddMusic(const char* path) {
  if (!mMusic) {
    mMusic = new LongSound(path);
    return;
  }
  throw("music already loaded, use remove music first");
}

void Interface::RemoveMusic() {
  if (mMusic) {
    delete mMusic;
    mMusic = NULL;
    return;
  }
  throw("no music to remove");
}

void Interface::PlayMusic() {
  if (mMusic) {
    mMusic->Play();
    return;
  }
  throw("no music to play");
}

void Interface::PauseMusic() {
  if (mMusic) {
    mMusic->Pause();
    return;
  }
  throw("no music loaded");
}

void Interface::ResumeMusic() {
  if (mMusic) {
    mMusic->Resume();
    return;
  }
  throw("no music loaded");
}

void Interface::StopMusic() {
  if (mMusic) {
    mMusic->Stop();
    return;
  }
  throw("no music loaded");
}
void Interface::SetMusicVolume(float new_vol) {
  if (mMusic) {
    mMusic->SetVolume(new_vol);
    return;
  }
  throw("no music loaded");
}

void Interface::SetCursorToHidden() noexcept {
  if (mWindow)
    mWindow->SetCursorToHidden();
}

void Interface::SetCursorToDisabled() noexcept {
  if (mWindow)
    mWindow->SetCursorToDisabled();
}

void Interface::SetCursorToNormal() noexcept {
  if (mWindow)
    mWindow->SetCursorToNormal();
}

void Interface::SetIMGUI(const bool value) {
  if (!value) {
    if (mIMGUI) {
      delete mIMGUI;
      mIMGUI = NULL;
    }
  }
  else {
    if (mIMGUI) {
      return;
    }
    else {
      mIMGUI = new imGUI();
      mIMGUI->InitOpenGL(mWindow->mGLFWwindow);
    }
  }
}

void Interface::SetWindowClearColor(glm::vec3 color) noexcept {
  OGLGraphics::SetViewportClearColor(color);
}

// returns -1 if there is no window
int Interface::GetWindowWidth() noexcept {
  if (!mWindow) return -1;
  return mWindow->GetCurrentWidth();
}

// returns -1 if there is no window
int Interface::GetWindowHeight() noexcept {
  if (!mWindow) return -1;
  return mWindow->GetCurrentHeight();
}

// changes the window title, does nothing if window is null
void Interface::SetWindowTitle(const char* name) noexcept {
  if (!mWindow) return;
  // todo: improve efficiency
  auto temp = mWindow->GetModifiableWindowOptions();
  temp->_title = name;
  mWindow->ApplyChanges();
}

// toggles fullscreen as expected, does nothign if window is null
void Interface::ToggleWindowFullscreen(bool try_borderless) noexcept {
  if (!mWindow) return;
  auto temp = mWindow->GetModifiableWindowOptions();

  if (temp->_windowing_mode == WINDOW_MODE::WINDOWED) {
    if (try_borderless) {
      temp->_windowing_mode = WINDOW_MODE::FULLSCREEN_BORDERLESS;
    }
    else {
      temp->_windowing_mode = WINDOW_MODE::FULLSCREEN;
    }
  }
  else {  // turn off fullscreen
    temp->_windowing_mode = WINDOW_MODE::WINDOWED_DEFAULT;
  }


  mWindow->ApplyChanges();
}

unsigned int Interface::AddToOnBegin(void(*function)()) {
  static unsigned int next_begin_id = 0;
  next_begin_id++;
  //onStart.emplace(next_begin_id, function);
  onStart.insert(std::make_pair(next_begin_id, function));
  return next_begin_id;
}

unsigned int Interface::AddToUpdate(void(*function)(float)) {
  static unsigned int next_deltaupdate_id = 0;
  next_deltaupdate_id++;
  onUpdate.emplace(next_deltaupdate_id, function);
  return next_deltaupdate_id;
}

unsigned int Interface::AddToImGuiUpdate(void(*function)()) {
  static unsigned int next_imgui_id = 0;
  next_imgui_id++;
  onImGuiUpdate.emplace(next_imgui_id, function);
  return next_imgui_id;
}

unsigned int Interface::AddToScrollHandling(void(*function)(MouseScrollWheel&)) {
  static unsigned int next_scrollhandling_id = 0;
  next_scrollhandling_id++;
  onScrollHandling.emplace(next_scrollhandling_id, function);
  return next_scrollhandling_id;
}

unsigned int Interface::AddToKeyHandling(void(*function)(KeyboardButtons&)) {
  static unsigned int next_keyhandling_id = 0;
  next_keyhandling_id++;
  onKeyHandling.emplace(next_keyhandling_id, function);
  return next_keyhandling_id;
}

unsigned int Interface::AddToMouseHandling(void(*function)(MouseCursorPos&)) {
  static unsigned int next_mousehandling_id = 0;
  next_mousehandling_id++;
  onMouseHandling.emplace(next_mousehandling_id, function);
  return next_mousehandling_id;
}

unsigned int Interface::AddToMouseButtonHandling(void(*function)(MouseButtons&)) {
  static unsigned int next_mouseButtonhandling_id = 0;
  next_mouseButtonhandling_id++;
  onMouseButtonHandling.emplace(next_mouseButtonhandling_id, function);
  return next_mouseButtonhandling_id;
}

unsigned int Interface::AddToOnTeardown(void(*function)()) {
  static unsigned int next_teardown_id = 0;
  next_teardown_id++;
  onQuit.emplace(next_teardown_id, function);
  return next_teardown_id;
}

bool Interface::RemoveFromOnBegin(unsigned int r_id) {
  return static_cast<bool>(onStart.erase(r_id));
}

bool Interface::RemoveFromUpdate(unsigned int r_id) {
  return static_cast<bool>(onUpdate.erase(r_id));
}

bool Interface::RemoveFromImGuiUpdate(unsigned int r_id) {
  return static_cast<bool>(onImGuiUpdate.erase(r_id));
}

bool Interface::RemoveFromScrollHandling(unsigned int r_id) {
  return static_cast<bool>(onScrollHandling.erase(r_id));
}

bool Interface::RemoveFromKeyHandling(unsigned int r_id) {
  return static_cast<bool>(onKeyHandling.erase(r_id));
}

bool Interface::RemoveFromMouseHandling(unsigned int r_id) {
  return static_cast<bool>(onMouseHandling.erase(r_id));
}

bool Interface::RemoveFromMouseButtonHandling(unsigned int r_id) {
  return static_cast<bool>(onMouseButtonHandling.erase(r_id));
}

bool Interface::RemoveFromTeardown(unsigned int r_id) {
  return static_cast<bool>(onQuit.erase(r_id));
}

} // end namespace AA

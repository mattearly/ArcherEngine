#include "../include/AAEngine/Interface.h"
#include "../include/AAEngine/Scene/Camera.h"
#include "../include/AAEngine/OS/Interface/Window.h"
#include "../include/AAEngine/Mesh/Prop.h"
#include "../include/AAEngine/Mesh/AnimProp.h"
#include "OS/OpenGL/Graphics.h"
#include "OS/OpenGL/InternalShaders/Init.h"
#include "Physics/NVidiaPhysx.h"
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

bool Interface::Init() {
  if (isInit)
    return false;
  mWindow = std::make_shared<Window>();
  SetIMGUI(true);
  SoundDevice::Init();
  InternalShaders::Init();
  auto physics_impl = NVidiaPhysx::Get();  // returns a pointer to implementation, ignored here
  isInit = true;
  return true;
}

bool Interface::Init(const WindowOptions& winopts) {
  if (isInit)
    return false;
  mWindow = std::make_shared<Window>(winopts);
  SetIMGUI(true);
  SoundDevice::Init();
  InternalShaders::Init();
  auto physics_impl = NVidiaPhysx::Get();  // returns a pointer to implementation, ignored here
  isInit = true;
  return true;
}

bool Interface::Init(std::shared_ptr<WindowOptions> winopts) {
  if (isInit)
    return false;
  mWindow = std::make_shared<Window>(winopts);
  SetIMGUI(true);
  SoundDevice::Init();
  InternalShaders::Init();
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
    pre_render();
    render();
    post_render();
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

// resets all vars but 
// not the window,
// base shaders,
// and renderer
void Interface::SoftReset() noexcept {
  // run user preferred functions first
  for (auto& oTD : onQuit) {
    oTD.second();
  }

  mSimulateWorldPhysics = false;

  mCameras.clear();

  // delete all the meshes and textures from GPU memory
  for (const auto& p : mProps) {
    p->RemoveCache();
  }
  mProps.clear();
  for (const auto& ap : mAnimatedProps) {
    ap->RemoveCache();
  }
  mAnimatedProps.clear();
  mAnimation.clear();

  RemoveSunLight();

  for (auto& pointlight : mPointLights) {
    RemovePointLight(pointlight->id);
  }

  for (auto& spotlight : mSpotLights) {
    RemoveSpotLight(spotlight->id);
  }

  if (mMusic) {
    RemoveMusic();
  }

  mSoundEffects.clear();
  mSpeakers.clear();

  onStart.clear();
  onUpdate.clear();
  onScrollHandling.clear();
  onKeyHandling.clear();
  onMouseHandling.clear();
  onMouseButtonHandling.clear();
  onQuit.clear();

  mWindow->SetCursorToNormal();
}

//
// Camera Access
//
unsigned int Interface::AddCamera(const int w, const int h) {
  mCameras.emplace_back(std::move(std::make_shared<Camera>((w < 0) ? 0 : w, (h < 0) ? 0 : h)));

  // sort by render depth if there is more than 1 camera
  if (mCameras.size() > 1) {
    std::sort(mCameras.begin(), mCameras.end(), [](auto a, auto b) {
      return (a->GetRenderDepth() < b->GetRenderDepth());
      });
  }

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

  if (before_size != after_size) {
    return true;  // success remove
  }

  return false;   // fail remove
}

std::weak_ptr<Camera> Interface::GetCamera(uidtype camId) {
  for (auto& cam : mCameras) {
    if (cam->GetUID() == camId) {
      return cam;
    }
  }

  throw("cam id doesn't exist or is invalid");
}

//
// Props Access
//
unsigned int Interface::AddProp(const char* path, const glm::vec3 location, const glm::vec3 scale) {
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

[[nodiscard]] std::weak_ptr<Prop> Interface::GetProp(const unsigned int id) const {
  for (auto& prop : mProps) {
    if (prop->GetUID() == id) {
      return prop;
    }
  }
  throw(-9999);
}

//
// Anim Props Access
//
unsigned int Interface::AddAnimProp(const char* path, glm::vec3 starting_location, glm::vec3 starting_scale) {
  mAnimatedProps.emplace_back(std::make_shared<AnimProp>(path));
  mAnimatedProps.back()->spacial_data.MoveTo(starting_location);
  mAnimatedProps.back()->spacial_data.ScaleTo(starting_scale);
  return mAnimatedProps.back()->GetUID();
}

bool Interface::RemoveAnimProp(const unsigned int id) {
  // remove cache (or decrement count of loaded in when multiloading)
  for (auto& prop : mAnimatedProps) {
    if (prop->GetUID() == id) {
      prop->RemoveCache();
    }
  }

  // the actual remove
  auto before_size = mAnimatedProps.size();

  auto ret_it = mAnimatedProps.erase(
    std::remove_if(
      mAnimatedProps.begin(),
      mAnimatedProps.end(),
      [&](auto& prop) { return prop->GetUID() == id; }),
    mAnimatedProps.end());

  auto after_size = mAnimatedProps.size();

  // return true if successful removal, false otherwise
  return (before_size != after_size);
}

std::weak_ptr<AnimProp> Interface::GetAnimProp(const unsigned int anim_prop_id) const {
  for (auto& prop : mAnimatedProps) {
    if (prop->GetUID() == anim_prop_id) {
      return prop;
    }
  }
  throw("prop id doesn't exist or is invalid");
}



unsigned int Interface::GetAnimPropBoneCount_testing(const unsigned int anim_prop_id) {
  for (auto& prop : mAnimatedProps) {
    if (prop->GetUID() == anim_prop_id)
      return (unsigned int)prop->m_Skeleton.m_Bones.size();
  }
  return 0;
}


//
// Animation Access
//
unsigned int Interface::AddAnimation(const char* path, const unsigned int anim_prop_id) {
  for (auto& prop : mAnimatedProps) {
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
  for (auto& animprop : mAnimatedProps) {
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


//
// Physics Integration Access
//
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


//
// Lights Interface
//
void Interface::SetSunLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) {
  if (!mSunLight) {
    mSunLight = std::make_shared<SunLight>(dir, amb, diff, spec);
  } else {
    mSunLight->Direction = dir;
    mSunLight->Ambient = amb;
    mSunLight->Diffuse = diff;
    mSunLight->Specular = spec;
  }

  auto uber_shader = InternalShaders::Uber::Get();
  uber_shader->SetInt("u_is_dir_light_on", 1);
  uber_shader->SetVec3("u_dir_light.Direction", mSunLight->Direction);
  uber_shader->SetVec3("u_dir_light.Ambient", mSunLight->Ambient);
  uber_shader->SetVec3("u_dir_light.Diffuse", mSunLight->Diffuse);
  uber_shader->SetVec3("u_dir_light.Specular", mSunLight->Specular);
}

void Interface::SetSunLightShadows(bool on_or_off) noexcept {
  if (!mSunLight) return;
  mSunLight->Shadows = on_or_off;
}

std::weak_ptr<SunLight> Interface::GetSunLight() noexcept {
  return mSunLight;
}

void Interface::RemoveSunLight() {
  if (mSunLight) {
    //assert(DefaultShaders::Get());
    //InternalShaders::Uber::Get()->Use();
    InternalShaders::Uber::Get()->SetInt("u_is_dir_light_on", 0);
    mSunLight.reset();
  }
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
    constant = linear = quadratic = ambient = diffuse = specular = position = "u_point_lights[";
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

    //assert(DefaultShaders::Get());
    //DefaultShaders::Get()->Use();
    OGLShader* shader_ref = InternalShaders::Uber::Get();
    shader_ref->SetVec3(position, mPointLights.back()->Position);
    shader_ref->SetFloat(constant, mPointLights.back()->Constant);
    shader_ref->SetFloat(linear, mPointLights.back()->Linear);
    shader_ref->SetFloat(quadratic, mPointLights.back()->Quadratic);
    shader_ref->SetVec3(ambient, mPointLights.back()->Ambient);
    shader_ref->SetVec3(diffuse, mPointLights.back()->Diffuse);
    shader_ref->SetVec3(specular, mPointLights.back()->Specular);
    shader_ref->SetInt("u_num_point_lights_in_use", static_cast<int>(new_point_size + 1));
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
    //DefaultShaders::Get()->Use();
    InternalShaders::Uber::Get()->SetInt("u_num_point_lights_in_use", static_cast<int>(after_size));

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

void Interface::MovePointLight(int which, glm::vec3 new_pos) {
  if (which < 0)
    throw("dont");

  int loc_in_vec = 0;
  for (auto& pl : mPointLights) {
    if (pl->id == which) {
      pl->Position = new_pos;
      std::stringstream ss;
      ss << loc_in_vec;
      std::string position = "u_point_lights[" + ss.str() + "].Position";
      //DefaultShaders::Get()->Use();
      InternalShaders::Uber::Get()->SetVec3(position.c_str(), pl->Position);
      return;
    }
    loc_in_vec++;
  }
  throw("u messed up");
}

void Interface::ChangePointLight(int which, glm::vec3 new_pos, float new_constant, float new_linear, float new_quad, glm::vec3 new_amb, glm::vec3 new_diff, glm::vec3 new_spec) {
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
        ambient = constant = diffuse = linear = quadrat = specular = pos = "u_point_lights[";
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

        //DefaultShaders::Get()->Use();
        OGLShader* shader_ref = InternalShaders::Uber::Get();
        shader_ref->SetVec3(pos, pl->Position);
        shader_ref->SetFloat(constant, pl->Constant);
        shader_ref->SetFloat(linear, pl->Linear);
        shader_ref->SetFloat(quadrat, pl->Quadratic);
        shader_ref->SetVec3(ambient, pl->Ambient);
        shader_ref->SetVec3(diffuse, pl->Diffuse);
        shader_ref->SetVec3(specular, pl->Specular);
      }
      return;
    }
    loc_in_vec++;
  }
  throw("u messed up");
}

void Interface::DebugLightIndicatorsOnOrOff(const bool& tf) {
  mDebugLightIndicators = tf;
}

// Spot Light
int Interface::AddSpotLight(glm::vec3 pos, glm::vec3 dir, float inner, float outer, float constant, float linear, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec) {
  if (mSpotLights.size() == MAXSPOTLIGHTS) {
    throw("too many spot lights");
  }

  mSpotLights.emplace_back(std::move(std::make_shared<SpotLight>(pos, dir, inner, outer, constant, linear, quad, amb, diff, spec)));
  auto new_spot_loc = mSpotLights.size() - 1;

  // push changes to shader
  {
    std::string pos, ambient, constant, cutoff, ocutoff, diffuse, direction, linear, quadrat, specular;
    ambient = constant = cutoff = ocutoff = diffuse = direction = linear = quadrat = specular = pos = "u_spot_lights[";
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

    //assert(DefaultShaders::Get());
    //DefaultShaders::Get()->Use();
    OGLShader* shader_ref = InternalShaders::Uber::Get();
    shader_ref->SetVec3(pos, mSpotLights.back()->Position);
    shader_ref->SetFloat(cutoff, mSpotLights.back()->CutOff);
    shader_ref->SetFloat(ocutoff, mSpotLights.back()->OuterCutOff);
    shader_ref->SetVec3(direction, mSpotLights.back()->Direction);
    shader_ref->SetFloat(constant, mSpotLights.back()->Constant);
    shader_ref->SetFloat(linear, mSpotLights.back()->Linear);
    shader_ref->SetFloat(quadrat, mSpotLights.back()->Quadratic);
    shader_ref->SetVec3(ambient, mSpotLights.back()->Ambient);
    shader_ref->SetVec3(diffuse, mSpotLights.back()->Diffuse);
    shader_ref->SetVec3(specular, mSpotLights.back()->Specular);
    shader_ref->SetInt("u_num_spot_lights_in_use", static_cast<int>(new_spot_loc + 1));
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
    //DefaultShaders::Get()->Use();
    InternalShaders::Uber::Get()->SetInt("u_num_spot_lights_in_use", static_cast<int>(after_size));

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

void Interface::MoveSpotLight(int which, glm::vec3 new_pos, glm::vec3 new_dir) {
  if (which < 0)
    throw("dont");

  int loc_in_vec = 0;
  for (auto& sl : mSpotLights) {
    if (sl->id == which) {
      sl->Position = new_pos;
      sl->Direction = new_dir;
      //DefaultShaders::Get()->Use();
      std::stringstream ss;
      ss << loc_in_vec;
      std::string position = "u_spot_lights[" + ss.str() + "].Position";
      std::string direction = "u_spot_lights[" + ss.str() + "].Direction";
      InternalShaders::Uber::Get()->SetVec3(position, sl->Position);
      InternalShaders::Uber::Get()->SetVec3(direction, sl->Direction);
      return;
    }
    loc_in_vec++;
  }
  throw("u messed up");
}

void Interface::ChangeSpotLight(int which, glm::vec3 new_pos, glm::vec3 new_dir, float new_inner, float new_outer, float new_constant, float new_linear, float new_quad, glm::vec3 new_amb, glm::vec3 new_diff, glm::vec3 new_spec) {
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
        ambient = constant = cutoff = ocutoff = diffuse = direction = linear = quadrat = specular = pos = "u_spot_lights[";
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


        //DefaultShaders::Get()->Use();
        OGLShader* shader_ref = InternalShaders::Uber::Get();
        shader_ref->SetVec3(pos, sl->Position);
        shader_ref->SetFloat(cutoff, sl->CutOff);
        shader_ref->SetFloat(ocutoff, sl->OuterCutOff);
        shader_ref->SetVec3(direction, sl->Direction);
        shader_ref->SetFloat(constant, sl->Constant);
        shader_ref->SetFloat(linear, sl->Linear);
        shader_ref->SetFloat(quadrat, sl->Quadratic);
        shader_ref->SetVec3(ambient, sl->Ambient);
        shader_ref->SetVec3(diffuse, sl->Diffuse);
        shader_ref->SetVec3(specular, sl->Specular);
      }
      return;
    }
    loc_in_vec++;
  }
  throw("u messed up");
}

// Sound Effects
// todo: rework so we can remove them properly
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

// todo: rework so we can remove them properly
void Interface::RemoveSoundEffect(int soundId) {
  throw("oh noes! this doesn't actually work yet");

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

void Interface::SetIMGUI(const bool value) {
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

void Interface::SetWindowClearColor(glm::vec3 color) noexcept {
  OpenGL::SetViewportClearColor(color);
}

std::weak_ptr<Window> Interface::GetWindow() {
  return mWindow;
}

// changes the window title, does nothing if window is null
void Interface::SetWindowTitle(const char* name) noexcept {
  if (!mWindow) return;
  // todo: improve efficiency
  auto temp = mWindow->get_and_note_window_options();
  auto shared = temp.lock();
  shared->_title = name;
  mWindow->apply_new_window_option_changes();
}

// toggles fullscreen as expected, does nothign if window is null
void Interface::ToggleWindowFullscreen(bool try_borderless) noexcept {
  if (!mWindow) return;
  auto weak = mWindow->get_and_note_window_options();
  auto shared = weak.lock();
  if (shared->_windowing_mode == WINDOW_MODE::WINDOWED || shared->_windowing_mode == WINDOW_MODE::MAXIMIZED || shared->_windowing_mode == WINDOW_MODE::WINDOWED_DEFAULT) {
    if (try_borderless) {
      shared->_windowing_mode = WINDOW_MODE::FULLSCREEN_BORDERLESS;
    } else {
      shared->_windowing_mode = WINDOW_MODE::FULLSCREEN;
    }
  } else {  // turn off fullscreen
    shared->_windowing_mode = WINDOW_MODE::WINDOWED_DEFAULT;
  }

  mWindow->apply_new_window_option_changes();
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

unsigned int Interface::AddToOnQuit(void(*function)()) {
  static unsigned int next_teardown_id = 0;
  next_teardown_id++;
  onQuit.emplace(next_teardown_id, function);
  return next_teardown_id;
}

bool Interface::RemoveFromOnBegin(unsigned int r_id) {
  return static_cast<bool>(onStart.erase(r_id));
}

bool Interface::RemoveFromOnUpdate(unsigned int r_id) {
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

bool Interface::RemoveFromOnQuit(unsigned int r_id) {
  return static_cast<bool>(onQuit.erase(r_id));
}

void Interface::ClearAllRuntimeLamdaFunctions() {
  onStart.clear();
  onUpdate.clear();
  onImGuiUpdate.clear();
  onScrollHandling.clear();
  onKeyHandling.clear();
  onMouseHandling.clear();
  onMouseButtonHandling.clear();
  onQuit.clear();
}

} // end namespace AA

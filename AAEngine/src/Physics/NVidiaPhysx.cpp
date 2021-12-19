#include "NVidiaPhysx.h"
#include "../OS/OpenGL/OGLShader.h"
#include <string>

using namespace physx;

namespace AA {


// Singleton Implementation
[[nodiscard]] NVidiaPhysx* NVidiaPhysx::Get() {
  static NVidiaPhysx* physics_singleton_impl = nullptr;
  if (!physics_singleton_impl) {
    physics_singleton_impl = new NVidiaPhysx();
  }
  return physics_singleton_impl;
}


// helper function
void SetupDefaultRigidDynamic(PxRigidDynamic& body, bool kinematic = false) {
  body.setActorFlag(PxActorFlag::eVISUALIZATION, true);
  body.setAngularDamping(0.5f);
  body.setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);
}

PxRigidDynamic* NVidiaPhysx::CreateBox(const PxVec3& pos, const PxVec3& dims, const PxVec3* linVel, PxReal density) {
  PxSceneWriteLock scopedLock(*mScene);
  PxRigidDynamic* box = PxCreateDynamic(*mPhysics, PxTransform(pos), PxBoxGeometry(dims), *mMaterial, density);
  //PX_ASSERT(box);

  SetupDefaultRigidDynamic(*box);

  mScene->addActor(*box);
  addPhysicsActors(box);

  if (linVel)
    box->setLinearVelocity(*linVel);

  //createRenderObjectsFromActor(box, material);

  return box;
}

PxRigidDynamic* NVidiaPhysx::CreateSphere(const PxVec3& pos, PxReal radius, const PxVec3* linVel, PxReal density) {
  PxSceneWriteLock scopedLock(*mScene);
  PxRigidDynamic* sphere = PxCreateDynamic(*mPhysics, PxTransform(pos), PxSphereGeometry(radius), *mMaterial, density);
  //PX_ASSERT(sphere);

  SetupDefaultRigidDynamic(*sphere);
  mScene->addActor(*sphere);
  addPhysicsActors(sphere);

  if (linVel)
    sphere->setLinearVelocity(*linVel);

  //createRenderObjectsFromActor(sphere, material);

  return sphere;
}

PxRigidDynamic* NVidiaPhysx::CreateCapsule(const PxVec3& pos, PxReal radius, PxReal halfHeight, const PxVec3* linVel, PxReal density) {
  PxSceneWriteLock scopedLock(*mScene);
  const PxQuat rot = PxQuat(PxIdentity);
  PX_UNUSED(rot);

  PxRigidDynamic* capsule = PxCreateDynamic(*mPhysics, PxTransform(pos), PxCapsuleGeometry(radius, halfHeight), *mMaterial, density);
  //PX_ASSERT(capsule);

  SetupDefaultRigidDynamic(*capsule);
  mScene->addActor(*capsule);
  addPhysicsActors(capsule);

  if (linVel)
    capsule->setLinearVelocity(*linVel);

  //createRenderObjectsFromActor(capsule, material);

  return capsule;
}

PxRigidStatic* NVidiaPhysx::CreateGroundPlane(const physx::PxVec3 normal, PxReal distance) {
  physx::PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, physx::PxPlane(normal, distance), *mMaterial);
  mScene->addActor(*groundPlane);
  return groundPlane;
}

NVidiaPhysx::NVidiaPhysx() {
  // Init physics
  mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
  if (!mFoundation) throw("PxCreateFoundation failed!");

  mPvd = PxCreatePvd(*mFoundation);
  physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
  mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

  //mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(),true, mPvd);
  mToleranceScale.length = 100;        // typical length of an object
  mToleranceScale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice
  mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);
  //mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale);
  physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
  sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
  mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
  sceneDesc.cpuDispatcher = mDispatcher;
  sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
  mScene = mPhysics->createScene(sceneDesc);


  physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
  if (pvdClient) {
    pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
    pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
    pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
  }

  mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
  //physx::PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, physx::PxPlane(0, 1, 0, 99), *mMaterial);
  //mScene->addActor(*groundPlane);
}

void NVidiaPhysx::removeActor(PxRigidActor* actor) {
  std::vector<PxRigidActor*>::iterator actorIter = std::find(mPhysicsActors.begin(), mPhysicsActors.end(), actor);
  if (actorIter != mPhysicsActors.end()) {
    mPhysicsActors.erase(actorIter);
    actor->release();
  }
}

// delete physx
NVidiaPhysx::~NVidiaPhysx() {
  if (mScene) {
    mScene->release();
    mScene = NULL;
  }
  if (mDispatcher) {
    mDispatcher->release();
    mDispatcher = NULL;
  }
  if (mPhysics) {
    mPhysics->release();
    mPhysics = NULL;
  }
  if (mPvd) {
    physx::PxPvdTransport* transport = mPvd->getTransport();
    mPvd->release();
    mPvd = NULL;
    if (transport) {
      transport->release();
      transport = NULL;
    }
  }
  if (mFoundation) {
    mFoundation->release();
    mFoundation = NULL;
  }
}

#ifdef _DEBUG
static OGLShader* physics_debug_shader = NULL;
void NVidiaPhysx::DrawDebug(const std::shared_ptr<Camera> cam) {
  if (physics_debug_shader) {
    physics_debug_shader->SetMat4("u_projection_matrix", cam->mProjectionMatrix);
    physics_debug_shader->SetMat4("u_view_matrix", cam->mViewMatrix);

  } else {
    const std::string vert = R"(
#version 430 core
in(location=0) vec3 inPos;
uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;
void main() {
  gl_Position = u_projection_matrix * u_view_matrix * u_model_matrix * vec4(inPos, 1.0);
})";
    const std::string frag = R"(
#version 430 core
out vec4 frag_color;
void main(){
  frag_color = vec4(color, 1.0);
})";
    physics_debug_shader = new OGLShader(vert.c_str(), frag.c_str());
    DrawDebug(cam);
  }
}
#endif

void NVidiaPhysx::StepPhysics(float dt) {
  mScene->simulate(dt);
  mScene->fetchResults(true);
}

void NVidiaPhysx::addPhysicsActors(PxRigidActor* actor) {
  mPhysicsActors.push_back(actor);
}


}  // end namespace AA


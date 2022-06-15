#pragma once
#include <PxPhysicsAPI.h>
#include <vector>
#include <memory>
#ifdef _DEBUG
#include <Scene/Camera.h>
#endif
namespace AA {

/// <summary>
/// Abstractions from NVidiaPhysxAPI
/// Singleton with Lazy Init: Use Get() to initialize and/or to get access to
/// https://github.com/NVIDIAGameWorks/PhysX/blob/4.1/physx/samples/samplebase/PhysXSample.h
/// </summary>
class NVidiaPhysx {
public:

  static [[nodiscard]] NVidiaPhysx* Get();
  static void Shutdown();

  void StepPhysics(float dt);

  physx::PxRigidDynamic* CreateBox(const physx::PxVec3& pos, const physx::PxVec3& dims, const physx::PxVec3* linVel = NULL, physx::PxReal density = 1.0f);
  physx::PxRigidDynamic* CreateSphere(const physx::PxVec3& pos, physx::PxReal radius, const physx::PxVec3* linVel = NULL, physx::PxReal density = 1.0f);
  physx::PxRigidDynamic* CreateCapsule(const physx::PxVec3& pos, physx::PxReal radius, physx::PxReal halfHeight, const physx::PxVec3* linVel = NULL, physx::PxReal density = 1.0f);
  physx::PxRigidStatic* CreateGroundPlane(const physx::PxVec3 normal, physx::PxReal distance);

  //PxRigidDynamic* CreateConvex(const physx::PxVec3& pos, const physx::PxVec3* linVel = NULL, physx::PxReal density = 1.0f);

  void removeActor(physx::PxRigidActor* actor);

  ~NVidiaPhysx();

#ifdef _DEBUG
  void DrawDebug(const std::shared_ptr<Camera> cam);
#endif

private:
  // singleton instance, do not construct, call Get()
  NVidiaPhysx();

  physx::PxDefaultAllocator mDefaultAllocatorCallback;
  physx::PxDefaultErrorCallback mDefaultErrorCallback;
  physx::PxFoundation* mFoundation;
  physx::PxPhysics* mPhysics;
  physx::PxDefaultCpuDispatcher* mDispatcher;
  physx::PxScene* mScene;
  physx::PxMaterial* mMaterial;
  physx::PxPvd* mPvd;
  physx::PxTolerancesScale mToleranceScale;

  std::vector<physx::PxRigidActor*> mPhysicsActors;

  void addPhysicsActors(physx::PxRigidActor* actor);

};
}

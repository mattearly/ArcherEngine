#pragma once
#include <PxPhysicsAPI.h>
#include <vector>
namespace AA {
// Abstractions from physx

using namespace physx;

////https://github.com/NVIDIAGameWorks/PhysX/blob/4.1/physx/samples/samplebase/PhysXSample.h
//struct PhysXShape
//{
//  physx::PxRigidActor*	mActor;
//  physx::PxShape*		mShape;
//
//  PhysXShape(physx::PxRigidActor* actor, physx::PxShape* shape) : mActor(actor), mShape(shape) {}
//  PhysXShape(const PhysXShape& shape) : mActor(shape.mActor), mShape(shape.mShape) {}
//  bool operator<(const PhysXShape& shape) const { return mActor == shape.mActor ? mShape < shape.mShape : mActor < shape.mActor; }
//};

class Physics {
public:
  static Physics* Get();
  void StepPhysics(float dt);
  PxRigidDynamic* CreateBox(const PxVec3& pos, const PxVec3& dims, const PxVec3* linVel = NULL, PxReal density = 1.0f);

  PxRigidDynamic* CreateSphere(const PxVec3& pos, PxReal radius, const PxVec3* linVel = NULL, PxReal density = 1.0f);

  PxRigidDynamic* CreateCapsule(const PxVec3& pos, PxReal radius, PxReal halfHeight, const PxVec3* linVel = NULL, PxReal density = 1.0f);

  PxRigidStatic* CreateGroundPlane(const physx::PxVec3 normal, PxReal distance);

  //PxRigidDynamic* CreateConvex(const PxVec3& pos, const PxVec3* linVel = NULL, PxReal density = 1.0f);

  void removeActor(PxRigidActor* actor);

  ~Physics();
private:
  Physics();
  physx::PxDefaultAllocator mDefaultAllocatorCallback;
  physx::PxDefaultErrorCallback mDefaultErrorCallback;
  physx::PxFoundation* mFoundation;
  physx::PxPhysics* mPhysics;
  physx::PxDefaultCpuDispatcher* mDispatcher;
  physx::PxScene* mScene;
  physx::PxMaterial* mMaterial;
  physx::PxPvd* mPvd;
  physx::PxTolerancesScale mToleranceScale;

  std::vector<PxRigidActor*> mPhysicsActors;
  void addPhysicsActors(PxRigidActor* actor);
};
}

//#pragma once
//#include <AncientArcher/AncientArcher.h>
//#include <AncientArcher/Utility/SceneCheck.h>
//#include <AncientArcher/Utility/rand.h>
//#include <glm/glm.hpp>
//extern AA::AncientArcher instance;
//
//static bool physxIsSetup = false;
//void setphysxmodels() {
//  if (physxIsSetup) return;
//
//  // PHYSX bottom plane
//  instance.AddGroundPlane_tmp(glm::vec3(0, 1, 0), 99);
//  // a bunch of glowing cubes
//  for (unsigned int i = 0; i < 80; i += 2) {
//    int id = AA::AddProp("res/neon_cube.glb", true, glm::vec3(AA::NTKR(-5.f, 3.f), (float)i + .01f, AA::NTKR(-5.f, 3.f)));
//    AA::AddPropPhysics(id);
//  }
//  // a bunch of glowing spheres
//  for (unsigned int i = 0; i < 80; i += 2) {
//    int id = AA::AddProp("res/neon_sphere.glb", true, glm::vec3(AA::NTKR(-20.f, -18.f), (float)i + .01f, AA::NTKR(-20.f, -18.f)));
//    AA::AddPropPhysics(id, AA::COLLIDERTYPE::SPHERE);
//  }
//  instance.SimulateWorldPhysics(true);
//
//
//  physxIsSetup = true;
//}
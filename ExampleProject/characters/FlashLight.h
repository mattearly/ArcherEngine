#pragma once
#include <AncientArcher/AncientArcher.h>
#include <glm/glm.hpp>
static int flashlightsoundid = -1;
extern int character_cam;
extern AA::AncientArcher Engine;
struct FlashLight {
  bool      isOn = false;
  int       id = -1;
  glm::vec3 position = glm::vec3(0);
  glm::vec3 direction = glm::vec3(0, -1, 0);
  float     inner_radius = glm::cos(glm::radians(6.09f));
  float     outer_radius = glm::cos(glm::radians(21.09f));
  float     constant = 1.f;
  float     linear = 0.00006999f;
  float     quad = 0.00001399f;
  glm::vec3 ambient = glm::vec3(1.f);
  glm::vec3 diffuse = glm::vec3(1.f);
  glm::vec3 specular = glm::vec3(1.f);
  void turn_on() {
    position = Engine.GetCamPosition(character_cam);
    direction = Engine.GetCamFront(character_cam);
    if (isOn) return;
    if (flashlightsoundid == -1)  // lazy init
    {
      flashlightsoundid = Engine.AddSoundEffect("res/flashlightclick.wav");
      Engine.SetSoundEffectVolume(flashlightsoundid, .1998f);
    }
    id = Engine.AddSpotLight(position, direction, inner_radius, outer_radius, constant, linear, quad, ambient, diffuse, specular);
    isOn = true;
    Engine.PlaySoundEffect(flashlightsoundid, true);
  };
  void turn_off() {
    if (!isOn) return;
    Engine.RemoveSpotLight(id);
    id = -1;
    isOn = false;
    Engine.PlaySoundEffect(flashlightsoundid, true);
  };
  void lock_in_new_params() {
    if (id != -1)
      Engine.ChangeSpotLight(id, position, direction, inner_radius, outer_radius, constant, linear, quad, ambient, diffuse, specular);
  }
  void lock_in_movement() {
    if (id != -1)
      Engine.MoveSpotLight(id, position, direction);
  }
};

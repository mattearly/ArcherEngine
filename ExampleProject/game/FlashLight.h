#pragma once
#include <AncientArcher/AncientArcher.h>
#include <glm/glm.hpp>
static int flashlightsoundid = -1;
extern int character_cam;
extern AA::AncientArcher instance;
struct FlashLight {
  bool      isOn = false;
  int       id = -1;
  glm::vec3 position = glm::vec3(0);
  glm::vec3 direction = glm::vec3(0, -1, 0);
  float     inner_radius = glm::cos(glm::radians(12.05f));
  float     outer_radius = glm::cos(glm::radians(34.05f));
  float     constant = 1.f;
  float     linear = 0.007f;
  float     quad = 0.0002f;
  glm::vec3 ambient = glm::vec3(0.93f);
  glm::vec3 diffuse = glm::vec3(1.57f);
  glm::vec3 specular = glm::vec3(1.0f);
  void turn_on() {
    position = instance.GetCamPosition(character_cam);
    direction = instance.GetCamFront(character_cam);
    if (isOn) return;
    if (flashlightsoundid == -1)  // lazy init
    {
      flashlightsoundid = instance.AddSoundEffect("res/flashlightclick.wav");
      instance.SetSoundEffectVolume(flashlightsoundid, .15f);
    }
    id = instance.AddSpotLight(position, direction, inner_radius, outer_radius, constant, linear, quad, ambient, diffuse, specular);
    isOn = true;
    instance.PlaySoundEffect(flashlightsoundid, true);
  };
  void turn_off() {
    if (!isOn) return;
    instance.RemoveSpotLight(id);
    id = -1;
    isOn = false;
    instance.PlaySoundEffect(flashlightsoundid, true);
  };
  void lock_in_new_params() {
    instance.ChangeSpotLight(id, position, direction, inner_radius, outer_radius, constant, linear, quad, ambient, diffuse, specular);
  }
  void lock_in_movement() {
    instance.MoveSpotLight(id, position, direction);
  }
};

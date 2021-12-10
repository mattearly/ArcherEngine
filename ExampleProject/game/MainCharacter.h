#pragma once
#include <AncientArcher/AncientArcher.h>
#include <AncientArcher/Utility/rand.h>
#include "Player.h"
#include "FlashLight.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <imgui.h>

bool is_inventory_open = false;
bool is_fps_controls_set = false;
int character_cam = -1;
FlashLight flashlight;
unsigned int footstepId[4];
static glm::vec3 frontFacingDir;
static glm::vec3 rightFacingDir;
extern AA::AncientArcher instance;
static float inventory_toggle_timer = 0.f;
static int inventory_gui_id = -1;
static int camera_radius_light = -1;
static int open_sound_id = -1;
static int close_sound_id = -1;
static const float FOOTSTEPPLAYINTERVAL = 1.f;
static float flashlight_toggle_timer = 0.f;

// for our move speed controls
static const float DEFAULTMOVESPEED = 2.0f;
static float sprint_bonus = DEFAULTMOVESPEED * 2;
// for our move direction and speed
static float current_move_speed = DEFAULTMOVESPEED;
static float prevFlySpeed = current_move_speed;
static glm::vec3 moveDir = glm::vec3(0.f);

static struct MoveBlock {
  bool forward = 0, backwards = 0, left = 0, right = 0;
  bool sprint = 0;
} move;

//fpp mouse
static double lastX{ 0 }, lastY{ 0 };
static bool re_entering_fpp = true;

struct MainCharacter {
public:
  void Setup() {
    if (isInitialized) return;
    character_cam = instance.AddCamera();
    instance.SetCamMaxRenderDistance(character_cam, 6000.f);
    instance.SetCamPosition(character_cam, glm::vec3(-10.0f, 85.0f, 1000.0f));
    instance.SetCursorToDisabled(); // disabled to hide for first person

    // SETUP RADIUS LIGHT AROUND CAMERA
    //camera_radius_light = AA::AddPointLight(glm::vec3(0, 100, 400),
    //  1.f, //constant
    //  0.0014f, //linear
    //  0.000007f, // quad
    //  glm::vec3(.7f), //amb
    //  glm::vec3(.7f), //diff
    //  glm::vec3(0.5f)); //spec

    //flashlight.turn_on();

    footstepId[0] = instance.AddSoundEffect("../ExampleProject/res/footstep_1.wav");
    footstepId[1] = instance.AddSoundEffect("../ExampleProject/res/footstep_2.wav");
    footstepId[2] = instance.AddSoundEffect("../ExampleProject/res/footstep_3.wav");
    footstepId[3] = instance.AddSoundEffect("../ExampleProject/res/footstep_4.wav");
    instance.SetSoundEffectVolume(footstepId[0], .4f);
    instance.SetSoundEffectVolume(footstepId[1], .4f);
    instance.SetSoundEffectVolume(footstepId[2], .4f);
    instance.SetSoundEffectVolume(footstepId[3], .4f);

    // SETUP MOVEMENT SYSTEM
    frontFacingDir = instance.GetCamFront(character_cam);
    rightFacingDir = instance.GetCamRight(character_cam);

    // SETUP INVENTORY SYSTEM
    open_sound_id = instance.AddSoundEffect("res/open_inv.ogg");
    close_sound_id = instance.AddSoundEffect("res/close_inv.ogg");

    instance.AddToKeyHandling([](AA::KeyboardButtons& kb) { // set TAB to open/close Inventory
      const float cd = .5f;
      if (kb.tab && inventory_toggle_timer > cd) {
        if (!is_inventory_open) {                   // Open Inventory
          is_inventory_open = true;
          instance.SetCursorToNormal();
          if (inventory_gui_id == -1) {
            inventory_gui_id = instance.AddToImGuiUpdate([]() {
              ImGui::Begin("Inventory");
              ImGui::TextColored(ImVec4(1, 1, 0, 1), "All Items");

              glm::vec3 camera_pos = instance.GetCamPosition(character_cam);

              ImGui::Text("Pos: %.1f, %.1f, %.1f", camera_pos.x, camera_pos.y, camera_pos.z);


              ImGui::BeginChild("Scrolling");
              for (int n = 0; n < 5; n++)
                ImGui::Text("Item %04d", n);
              ImGui::EndChild();
              ImGui::End();
            });
          }
          instance.PlaySoundEffect(open_sound_id);
        } else {                                    // Close Inventory
          is_inventory_open = false;
          instance.SetCursorToDisabled();
          if (inventory_gui_id != -1) {
            instance.RemoveFromImGuiUpdate(inventory_gui_id);
            inventory_gui_id = -1;
          }
          re_entering_fpp = true;
          instance.PlaySoundEffect(close_sound_id);
        }
        inventory_toggle_timer = 0.f; // reset timer
      }
    });

    // set WASD key first person movement function
    instance.AddToKeyHandling([](AA::KeyboardButtons& key) {
      if (key.w) {
        move.forward = true;
      } else if (!key.w) {
        move.forward = false;
      }

      if (key.s) {
        move.backwards = true;
      } else if (!key.s) {
        move.backwards = false;
      }

      if (key.a) {
        move.left = true;
      } else if (!key.a) {
        move.left = false;
      }

      if (key.d) {
        move.right = true;
      } else if (!key.d) {
        move.right = false;
      }

      if (key.leftShift) {
        move.sprint = true;
      } else /*if (!key.leftShift)*/ {
        move.sprint = false;
      }

    });

    // add first person mouse movement to change our view direction
    instance.AddToMouseHandling([](AA::MouseCursorPos& cursor) {
      if (is_inventory_open)
        return;

      const float SENSITIVITY = .1f;
      static double xDelta{ 0 }, yDelta{ 0 };
      if (re_entering_fpp) {
        lastX = cursor.xOffset;  // so it doesn't snap
        lastY = cursor.yOffset;
        if (cursor.xOffset != 0 && cursor.yOffset != 0) {
          re_entering_fpp = false;
        }
      }
      // reset 
      xDelta = 0, yDelta = 0;
      // get change
      xDelta = cursor.xOffset - lastX;
      yDelta = lastY - cursor.yOffset;
      // hold result
      lastX = cursor.xOffset;
      lastY = cursor.yOffset;
      // get a percent of the actual offset
      xDelta *= SENSITIVITY;
      yDelta *= SENSITIVITY;

      instance.ShiftCamPitchAndYaw(character_cam, yDelta, xDelta);

      frontFacingDir = instance.GetCamFront(character_cam);
      rightFacingDir = instance.GetCamRight(character_cam);

      if (flashlight.isOn) {
        flashlight.direction = frontFacingDir;
        flashlight.lock_in_movement();
      }

    });

    // default on flashlight
    flashlight.turn_on();
    // SETUP FLASHLIGHT TOGGLE
    instance.AddToKeyHandling([](AA::KeyboardButtons& kb) {
      const float cd = .25f;
      if (kb.f && flashlight_toggle_timer > cd) {
        if (flashlight.isOn) {
          flashlight.turn_off();
        } else {
          flashlight.turn_on();
        }
        flashlight_toggle_timer = 0.f; // reset timer on success
      }
    });

    isInitialized = true;
  }
  void Update(float dt) {
    inventory_toggle_timer += dt;  // update timer
    flashlight_toggle_timer += dt;

    moveDir = glm::vec3(0.f);
    if (move.forward) {
      moveDir += frontFacingDir;
    } else if (move.backwards) {
      moveDir -= frontFacingDir;
    }
    if (move.right) {
      moveDir += rightFacingDir;
    } else if (move.left) {
      moveDir -= rightFacingDir;
    }

    static float footstepCooldown = FOOTSTEPPLAYINTERVAL;
    footstepCooldown += (move.sprint) ? dt * 2.f : dt;
    if (move.forward || move.backwards || move.right || move.left) {
      moveDir.y = 0.f;
      float frame_calculated_velocity = dt * (move.sprint) ? sprint_bonus + current_move_speed : current_move_speed;
      instance.ShiftCamPosition(character_cam, moveDir * frame_calculated_velocity);
      glm::vec3 cam_loc = instance.GetCamPosition(character_cam);
      if (camera_radius_light != -1) {
        instance.MovePointLight(camera_radius_light, cam_loc);
      }
      if (flashlight.isOn) {
        flashlight.position = cam_loc;
        flashlight.lock_in_movement();
      }
      if (footstepCooldown > FOOTSTEPPLAYINTERVAL) {
        unsigned int random_index = AA::NTKR(0, 3);
        instance.PlaySoundEffect(footstepId[random_index], true);
        footstepCooldown = 0.f;
      }
    }
  }
  void Teardown() {
    // todo: add on quit triggers (if any)
  }
private:
  bool isInitialized = false;
};

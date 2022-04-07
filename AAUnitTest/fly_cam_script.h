#pragma once
#include "test_globals.h"

// globals local to this file
bool fly_setup = false;
struct Move {
  bool forward = false;
  bool backwards = false;
  bool left = false;
  bool right = false;
  bool up = false;
  bool down = false;
  bool sprint = false;
} move;
bool is_cursor_on = true;
glm::vec3 move_diff{ 0 };
const float DEFAULTMOVESPEED = 16.f;
double FPP_SENSITIVITY = .1f;
double lastX{ 0 }, lastY{ 0 };
double xDelta{ 0 }, yDelta{ 0 };
bool UnprocessedMovements = false;
bool snap_to_center = false;
unsigned int cam_id_set_to = 0;
unsigned int fly_mouse_button_func = 0;
unsigned int fly_kb_button_func = 0;
unsigned int fly_mouse_handling_func = 0;
unsigned int fly_update_func = 0;
std::weak_ptr<AA::Window> fly_window;
std::weak_ptr<AA::Camera> fly_camera;

void setup_fpp_fly(unsigned int cam) {
  if (fly_setup) return;

  cam_id_set_to = cam;

  fly_mouse_button_func = g_aa_interface.AddToMouseButtonHandling([](AA::MouseButtons& mb) {
    if (mb.mouseButton2) {
      fly_window = g_aa_interface.GetWindow();
      std::shared_ptr<AA::Window> local_fly_window = fly_window.lock();
      if (!is_cursor_on) {
        local_fly_window->SetCursorToNormal();
        is_cursor_on = !is_cursor_on;
      } else {
        local_fly_window->SetCursorToDisabled();
        is_cursor_on = !is_cursor_on;
        snap_to_center = true;
      }
    }
    });

  fly_kb_button_func = g_aa_interface.AddToKeyHandling([](AA::KeyboardButtons& kb) {
    if (kb.w) { move.forward = true; } else { move.forward = false; }
    if (kb.s) { move.backwards = true; } else { move.backwards = false; }
    if (kb.a) { move.left = true; } else { move.left = false; }
    if (kb.d) { move.right = true; } else { move.right = false; }
    if (kb.spacebar) { move.up = true; } else { move.up = false; }
    if (kb.c) { move.down = true; } else { move.down = false; }
    if (kb.leftShift) { move.sprint = true; } else { move.sprint = false; }
    });

  fly_mouse_handling_func = g_aa_interface.AddToMouseHandling([](AA::MouseCursorPos& cursor) {
    if (is_cursor_on) return;

    if (snap_to_center) {
      lastX = cursor.xOffset;
      lastY = cursor.yOffset;
      snap_to_center = false;
    }

    xDelta = yDelta = 0;
    xDelta = cursor.xOffset - lastX;
    yDelta = lastY - cursor.yOffset;

    if (xDelta != 0.0 || yDelta != 0.0) {
      fly_camera = g_aa_interface.GetCamera(cam_id_set_to);
      std::shared_ptr<AA::Camera> local_fly_cam = fly_camera.lock();
      local_fly_cam->ShiftPitchAndYaw(yDelta * FPP_SENSITIVITY, xDelta * FPP_SENSITIVITY);

      lastX = cursor.xOffset;
      lastY = cursor.yOffset;
    }

    });

  fly_update_func = g_aa_interface.AddToUpdate([](float dt) {
    fly_camera = g_aa_interface.GetCamera(cam_id_set_to);
    if (move.forward) {
      std::shared_ptr<AA::Camera> local_fly_cam = fly_camera.lock();
      move_diff += local_fly_cam->GetFront();
      UnprocessedMovements = true;
    }
    if (move.backwards) {
      std::shared_ptr<AA::Camera> local_fly_cam = fly_camera.lock();
      move_diff -= local_fly_cam->GetFront();
      UnprocessedMovements = true;
    }
    if (move.left) {
      std::shared_ptr<AA::Camera> local_fly_cam = fly_camera.lock();
      move_diff -= local_fly_cam->GetRight();
      UnprocessedMovements = true;
    }
    if (move.right) {
      std::shared_ptr<AA::Camera> local_fly_cam = fly_camera.lock();
      move_diff += local_fly_cam->GetRight();
      UnprocessedMovements = true;
    }
    if (move.up || move.down) {
      UnprocessedMovements = true;
    }

    // movement detected - do the move logic
    if (UnprocessedMovements) {

      // walking around movements
      glm::vec3 amount_shifted(0);
      amount_shifted.x += move_diff.x * dt * DEFAULTMOVESPEED;
      amount_shifted.z += move_diff.z * dt * DEFAULTMOVESPEED;
      if (move.sprint) {
        amount_shifted.x *= 2.f;
        amount_shifted.z *= 2.f;
      }

      // going up or down logic
      if (move.up) {
        amount_shifted.y += dt * DEFAULTMOVESPEED;
      } else if (move.down) {
        amount_shifted.y -= dt * DEFAULTMOVESPEED;
      }

      // apply final positions
      std::shared_ptr<AA::Camera> local_fly_cam = fly_camera.lock();
      local_fly_cam->ShiftPosition(glm::vec3(amount_shifted.x, 0, amount_shifted.z));
      local_fly_cam->ShiftPosition(glm::vec3(0, amount_shifted.y, 0));

      // reset movement for next frame
      UnprocessedMovements = !UnprocessedMovements;
      move_diff = glm::vec3(0);
    }
    });

  fly_setup = true;
}

void turn_off_fly() {
  cam_id_set_to = 0;
  fly_setup = false;
  move.forward = false;
  move.backwards = false;
  move.left = false;
  move.right = false;
  move.up = false;
  move.down = false;
  move.sprint = false;
  is_cursor_on = true;
  move_diff = glm::vec3(0);
  FPP_SENSITIVITY = .1f;
  lastX = lastY = xDelta = yDelta = 0;
  UnprocessedMovements = false;
  snap_to_center = false;

  if (g_aa_interface.RemoveFromMouseButtonHandling(fly_mouse_button_func)) {
    fly_mouse_button_func = 0;
  }
  if (g_aa_interface.RemoveFromKeyHandling(fly_kb_button_func)) {
    fly_kb_button_func = 0;

  }
  if (g_aa_interface.RemoveFromMouseHandling(fly_mouse_handling_func)) {
    fly_mouse_handling_func = 0;
  }
  if (g_aa_interface.RemoveFromOnUpdate(fly_update_func)) {
    fly_update_func = 0;
  }

  fly_window.reset();
  fly_camera.reset();

}
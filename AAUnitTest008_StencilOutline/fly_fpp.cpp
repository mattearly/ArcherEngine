#include "pch.h"
extern AA::Interface instance;
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
glm::vec3 move_diff;
const int DEFAULTMOVESPEED = 3;
double FPP_SENSITIVITY = .1f;
double lastX{ 0 }, lastY{ 0 };
double xDelta{ 0 }, yDelta{ 0 };
bool UnprocessedMovements = false;
bool snap_to_center = false;

void setup_fpp_fly(unsigned int cam) {
  if (fly_setup) return;
  static auto s_cam_id = cam;
  //static auto cam = instance.GetCamera(cam_id);
  //static auto win = instance.GetWindow();
  instance.AddToMouseButtonHandling([](AA::MouseButtons& mb) {
    if (mb.mouseButton2) {
      if (!is_cursor_on) {
        instance.GetWindow()->SetCursorToNormal();
        is_cursor_on = !is_cursor_on;
      } else {
        instance.GetWindow()->SetCursorToDisabled();
        is_cursor_on = !is_cursor_on;
        snap_to_center = true;
      }
    }
    });

  instance.AddToKeyHandling([](AA::KeyboardButtons& kb) {
    if (kb.w) { move.forward = true; } else { move.forward = false; }
    if (kb.s) { move.backwards = true; } else { move.backwards = false; }
    if (kb.a) { move.left = true; } else { move.left = false; }
    if (kb.d) { move.right = true; } else { move.right = false; }
    if (kb.spacebar) { move.up = true; } else { move.up = false; }
    if (kb.c) { move.down = true; } else { move.down = false; }
    if (kb.leftShift) { move.sprint = true; } else { move.sprint = false; }
    });

  instance.AddToMouseHandling([](AA::MouseCursorPos& cursor) {
    if (is_cursor_on) return;

    if (snap_to_center) {
      lastX = cursor.xOffset;
      lastY = cursor.yOffset;
      snap_to_center = false;
    }

    xDelta = yDelta = 0;
    xDelta = cursor.xOffset - lastX;
    yDelta = lastY - cursor.yOffset;

    instance.GetCamera(s_cam_id)->ShiftPitchAndYaw(yDelta * FPP_SENSITIVITY, xDelta * FPP_SENSITIVITY);

    lastX = cursor.xOffset;
    lastY = cursor.yOffset;

    });

  instance.AddToUpdate([](float dt) {
    auto cam_ref = instance.GetCamera(s_cam_id);
    if (move.forward) {
      move_diff += cam_ref->GetFront();
      UnprocessedMovements = true;
    }
    if (move.backwards) {
      move_diff -= cam_ref->GetFront();
      UnprocessedMovements = true;
    }
    if (move.left) {
      move_diff -= cam_ref->GetRight();
      UnprocessedMovements = true;
    }
    if (move.right) {
      move_diff += cam_ref->GetRight();
      UnprocessedMovements = true;
    }
    if (UnprocessedMovements) {
      cam_ref->ShiftPosition(glm::vec3(move_diff.x, cam_ref->GetPosition().y, move_diff.z));
      UnprocessedMovements = !UnprocessedMovements;
      move_diff = glm::vec3(0);
    }
    });

  fly_setup = true;
}
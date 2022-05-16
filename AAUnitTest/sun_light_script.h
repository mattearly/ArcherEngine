#pragma once
#include "test_globals.h"

static bool control_setup = false;
static AA::Interface* interface_ref;

// globals local to this file
void setup_fpp_control(unsigned int sunlight_id_to_control, AA::Interface& interface) {
  if (control_setup) return;

  interface_ref = &interface;

  control_setup = true;
}

void turn_off_control() {
  control_setup = false;
  interface_ref = nullptr;
}
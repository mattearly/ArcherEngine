#pragma once
#include "Shadow.h"
#include "Uber.h"
#include "Stencil.h"
#include "Skycube.h"
#include "Icon.h"
namespace AA {
namespace InternalShaders {
static void Init() {
  Uber::Get();
  Skycube::Get();
  Stencil::Get();
  Shadow::Get();
  Icon::Get();
}

static void Shutdown() {
  Uber::Shutdown();
  Skycube::Shutdown();
  Stencil::Shutdown();
  Shadow::Shutdown();
  Icon::Shutdown();
}
}  // end namespace InternalShaders
}  // end namespace AA


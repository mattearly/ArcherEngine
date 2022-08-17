#pragma once
#include "Shadow.h"
#include "Uber.h"
#include "Stencil.h"
#include "Skycube.h"
#include "Basic.h"
namespace AA {
namespace InternalShaders {
static void Init() {
  Uber::Get();
  Skycube::Get();
  Stencil::Get();
  Shadow::Get();
  Basic::Get();
}

static void Shutdown() {
  Uber::Shutdown();
  Skycube::Shutdown();
  Stencil::Shutdown();
  Shadow::Shutdown();
  Basic::Shutdown();
}
}  // end namespace InternalShaders
}  // end namespace AA


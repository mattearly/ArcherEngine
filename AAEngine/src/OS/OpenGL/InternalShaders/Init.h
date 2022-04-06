#pragma once
#include "Shadow.h"
#include "Uber.h"
#include "Stencil.h"
#include "Skycube.h"
namespace AA {
namespace InternalShaders {
static void Init() {
  Uber::Get();
  Skycube::Get();
  Stencil::Get();
  Shadow::Get();
}
}
}


#pragma once
#include "Stats.h"
#include <string>
enum class ITEMTYPE { USABLE, EQUIPMENT };

struct Item {
  std::string name;
  ITEMTYPE    type;
  uint8_t     count;
  Stats       stats;
};

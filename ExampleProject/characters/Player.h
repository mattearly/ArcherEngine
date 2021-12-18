#pragma once
#include <string>
#include "Stats.h"

enum class ITEMTYPE { USABLE, EQUIPMENT };

struct Item {
  std::string name = "";
  ITEMTYPE    item_type = ITEMTYPE::USABLE;
  uint8_t     count = 0;
  Stats       magical_powers{};
};

struct Inventory {
  std::string bag_name = "Bag";
  std::vector<Item> slots;
};

class Player {
public:
  Stats stats;

  Inventory inventory;

  Item main_hand;
  Item off_hand;
  Item head, neck, mid, back, arms, hands, waist, legs, feet;
};
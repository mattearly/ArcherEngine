#pragma once
#include <string>
struct Stats {
  int hp = 0;
  int mp = 0;
  int energy = 0;
  int mental = 0;
  int armor = 0;
  int dodge = 0;
  int resistance = 0;
};
static void reset_stats(Stats& s) { s.hp = s.mp = s.energy = s.mental = s.armor = s.dodge = s.resistance = 0; }
struct Item : Stats {
  std::string name = "";
  std::string type = "";
  uint8_t     count = 0;
};
struct Inventory {
  static const uint8_t SLOTS = 12;
  std::string name = "";
  Item items[SLOTS] = {};
};
struct Equipped {
  Item main_hand;
  Item off_hand;
  Item head, neck, mid, back, arms, hands, waist, legs, feet;
};
class Player {
public:
  Stats stats;
  Equipped equipped;
  Inventory inventory;
};
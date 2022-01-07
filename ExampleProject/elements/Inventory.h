#pragma once
#include "Item.h"
#include <string>
#include <vector>

struct Inventory {
  std::string bag_name = "Bag";
  std::vector<Item> slots;
};

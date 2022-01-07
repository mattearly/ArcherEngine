#pragma once
struct Stats {
  int hp = 0;
  int mp = 0;
  int energy = 0;
  int speed = 0;
  int armor = 0;
  int dodge = 0;
  int resistance = 0;
};
static void reset_stats(Stats& s) { s.hp = s.mp = s.energy = s.speed = s.armor = s.dodge = s.resistance = 0; }
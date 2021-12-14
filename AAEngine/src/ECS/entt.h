#pragma once
#include <entt/entt.hpp>
namespace AA {

struct entity_id {
  using entity_type = std::uint32_t;
  static constexpr auto null = entt::null;

  constexpr entity_id(entity_type value = null) ENTT_NOEXCEPT
    : entt{ value } {}

  constexpr entity_id(const entity_id& other) ENTT_NOEXCEPT
    : entt{ other.entt } {}

  constexpr operator entity_type() const ENTT_NOEXCEPT {
    return entt;
  }

private:
  entity_type entt;
};

class Entt { 
  //singleton
  static entt::basic_registry<entity_id>* Get();

private:
  Entt();

  entt::basic_registry<entity_id> mRegistry;

};
}

#include "entt.h"
namespace AA {
static Entt* entt_instance;
entt::basic_registry<entity_id>* Entt::Get() {
  if (!entt_instance) {
    entt_instance = new Entt();
  }
  return &entt_instance->mRegistry;
}
}
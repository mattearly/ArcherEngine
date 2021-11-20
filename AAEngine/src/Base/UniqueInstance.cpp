#include "UniqueInstance.h"
namespace AA {
static unsigned int next_uid = 0;
unsigned int UniqueInstance::GetUID() const noexcept { return UniqueID; }
UniqueInstance::UniqueInstance() {
  UniqueID = ++next_uid;
};
}
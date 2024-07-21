#include "UniqueInstance.h"
namespace AA {
static uidtype next_uid = 0;
uidtype UniqueInstance::GetUID() const noexcept { return UniqueID; }
UniqueInstance::UniqueInstance() {
  UniqueID = ++next_uid;
};
}
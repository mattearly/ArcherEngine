#pragma once
#include <cstdint>
namespace AA {

typedef uint32_t uidtype;

class UniqueInstance {
public:
  virtual uidtype GetUID() const noexcept;
  UniqueInstance();
private:
  uidtype UniqueID;
};
}  // end namespace AA

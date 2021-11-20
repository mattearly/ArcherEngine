#pragma once
namespace AA {
class UniqueInstance {
public:
  virtual unsigned int GetUID() const noexcept;
  UniqueInstance();
private:
  unsigned int UniqueID;
};
}  // end namespace AA

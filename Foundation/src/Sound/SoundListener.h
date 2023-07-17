#pragma once
#include <AL/al.h>
#include <glm/glm.hpp>
namespace AA {
class SoundListener {
public:
  static SoundListener* Get();

  void SetMasterGain(const float& val);

  void SetPosition(const glm::vec3& pos);
  void SetPosition(const float& x, const float& y, const float& z);
  void SetLocation(const glm::vec3& pos);
  void SetLocation(const float& x, const float& y, const float& z);

  void SetOrientation(const glm::vec3& at, const glm::vec3& up = glm::vec3(0, 1, 0));
  void SetOrientation(
    const float& xat, const float& yat, const float& zat,
    const float& xup, const float& yup, const float& zup);

  void SetDistanceModel(ALint textureType);

protected:
  SoundListener();

};

} // end namespace AA
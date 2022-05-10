#pragma once
#include <glm/glm.hpp>

namespace AA {

struct DirectionalLight {
  glm::vec3 Direction, Ambient, Diffuse, Specular;
  DirectionalLight(glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);
  ~DirectionalLight();

  // directional shadows
  bool Shadows = true;

  // oglshadersettings
  const unsigned int& GetFBO() const;
  const unsigned int& GetTexID() const;
  unsigned int ShadowWidth;
  unsigned int ShadowHeight;
  float ShadowNearPlane, ShadowFarPlane;
  float ShadowOrthoSize;

};


/* Point Light Examples
* Distance  Constant  Linear  Quadratic
* 7         1.0       0.7     1.8
* 13        1.0       0.35    0.44
* 20        1.0       0.22    0.20
* 32        1.0       0.14    0.07
* 50        1.0       0.09    0.032
* 65        1.0       0.07    0.017
* 100       1.0       0.045   0.0075
* 160       1.0       0.027   0.0028
* 200       1.0       0.022   0.0019
* 325       1.0       0.014   0.0007
* 600       1.0       0.007   0.0002
* 3250      1.0       0.0014  0.000007
*/
struct PointLight {
  PointLight(glm::vec3 pos, float constant, float linear, float quad, glm::vec3 amb,
    glm::vec3 diff, glm::vec3 spec);

  glm::vec3 Position;
  float Constant, Linear, Quadratic;
  glm::vec3 Ambient, Diffuse, Specular;

  int id;
};

struct SpotLight {
  SpotLight(glm::vec3 pos, glm::vec3 dir, float inner, float outer, float constant,
    float linear, float quad, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec);
  glm::vec3 Position, Direction;
  float CutOff, OuterCutOff;
  float Constant, Linear, Quadratic;
  glm::vec3 Ambient, Diffuse, Specular;

  int id;

};

}  // end namespace AA
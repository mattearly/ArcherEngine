#pragma once
#include <glm/glm.hpp>

struct view_block {
  glm::mat4 u_projection_matrix;
  glm::mat4 u_view_matrix;
  glm::mat4 u_model_matrix;
};

constexpr auto MAX_BONES = 4;

struct animation_block {
  glm::mat4 u_final_bone_mats[MAX_BONES];
  int u_is_animating;
};

struct Material {
  int Albedo;
  int Specular;
  int Normal;
  int Emission;
  float Shininess;
};
struct DirectionalLight {
  glm::vec3 Direction;
  glm::vec3 Ambient, Diffuse, Specular;
};
struct PointLight {
  glm::vec3 Position;
  glm::vec3 Ambient, Diffuse, Specular;
  float Constant, Linear, Quadratic;
};
struct SpotLight {
  glm::vec3 Position, Direction;
  glm::vec3 Ambient, Diffuse, Specular;
  float Constant, Linear, Quadratic;
  float CutOff, OuterCutOff;
};

struct material_block {
  int Albedo;
  int Specular;
  int Normal;
  int Emission;
  float Shininess;
};

constexpr auto MAX_POINT_LIGHTS = 24;
constexpr auto MAX_SPOT_LIGHTS = 12;

struct lighting_block {
  glm::vec3 u_view_pos;
  int u_is_dir_light_on;
  DirectionalLight u_dir_light;
  PointLight u_point_lights[MAX_POINT_LIGHTS];
  SpotLight u_spot_lights[MAX_SPOT_LIGHTS];
  int u_num_point_lights_in_use;
  int u_num_spot_lights_in_use;
};

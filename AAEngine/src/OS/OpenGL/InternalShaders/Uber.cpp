#include "Uber.h"
#include <string>
namespace AA::InternalShaders {

static OGLShader* UBERSHADER = nullptr;

//todo: optimization - uniform buffers https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
void Uber::Init() {
  if (UBERSHADER)
    return;

  const std::string UBERSHADER_VERT_CODE =
    R"(
#version 430 core

layout(location=0)in vec3 inPos;
layout(location=1)in vec2 inTexUV;
layout(location=2)in vec3 inNorm;
layout(location=3)in ivec4 inBoneIds;
layout(location=4)in vec4 inWeights;

out VS_OUT {
  vec3 Pos;
  vec2 TexUV;
  vec3 Norm;
} vs_out;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_model_matrix;

uniform mat4 u_final_bone_mats[MAX_BONES];
uniform int u_is_animating;

void main(){
  vs_out.TexUV = inTexUV;
  vec4 totalPosition = vec4(0.0);

  if (u_is_animating > 0) {
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
      if(inBoneIds[i] == -1) continue;
      if(inBoneIds[i] >= MAX_BONES) {
          totalPosition = vec4(inPos, 1.0f);
          break;
      }
      vec4 localPosition = u_final_bone_mats[inBoneIds[i]] * vec4(inPos, 1.0);
      totalPosition += localPosition * inWeights[i];
    }
    vs_out.Pos = (u_model_matrix * totalPosition).xyz;
    vs_out.Norm = inNorm;
  } else {  // Not Animating
    mat3 normal_matrix = transpose(inverse(mat3(u_model_matrix)));
    vs_out.Pos = (u_model_matrix * vec4(inPos, 1.0)).xyz;
    vs_out.Norm = normalize(normal_matrix * inNorm);
    totalPosition = vec4(inPos, 1.0);
  }
  mat4 viewMatrix = u_view_matrix * u_model_matrix;
  gl_Position = u_projection_matrix * viewMatrix * totalPosition;
}
)";

  const std::string UBERSHADER_FRAG_CODE =
    R"(
#version 430 core

in VS_OUT
{
  vec3 Pos;
  vec2 TexUV;
  vec3 Norm;
} fs_in;

layout(location=0)out vec4 out_Color;

struct Material {
  sampler2D Albedo;
  sampler2D Specular;
  sampler2D Normal;
  sampler2D Emission;
  float Shininess;
};
struct DirectionalLight {
  vec3 Direction;
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;
};
struct PointLight {
  vec3 Position;
  float Constant, Linear, Quadratic;
  vec3 Ambient, Diffuse, Specular;
};
struct SpotLight {
  vec3 Position, Direction;
  float CutOff, OuterCutOff;
  float Constant, Linear, Quadratic;
  vec3 Ambient, Diffuse, Specular;
};

struct ReflectionModel {
  bool Phong;
  bool BlinnPhong;
};

const int MAXPOINTLIGHTS = 24; // if changed, needs to match on light controllers
const int MAXSPOTLIGHTS = 12;
const vec3 DEFAULTCOLOR = vec3(0.9,0.9,0.9);
uniform vec3 u_view_pos;
uniform int u_has_albedo_tex;
uniform int u_has_specular_tex;
uniform int u_has_normal_tex;
uniform int u_has_emission_tex;
uniform ReflectionModel u_reflection_model;
uniform Material u_material;
uniform int u_is_dir_light_on;
uniform DirectionalLight u_dir_light;
uniform PointLight u_point_lights[MAXPOINTLIGHTS];
uniform SpotLight u_spot_lights[MAXSPOTLIGHTS];
uniform int u_num_point_lights_in_use;
uniform int u_num_spot_lights_in_use;

vec3 CalculateDirLight(vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-u_dir_light.Direction);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.);

  // specular shading
  float spec;
  if (u_reflection_model.BlinnPhong) {
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), u_material.Shininess * 4);
  } else if (u_reflection_model.Phong) {
    vec3 reflectDir = reflect(-lightDir, normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.), u_material.Shininess);
  } else {
    spec = 0.0;
  }

  // combine results
  vec3 ambient;
  vec3 diffuse;
  if (u_has_albedo_tex > 0) { 
    ambient = u_dir_light.Ambient * texture(u_material.Albedo, fs_in.TexUV).rgb;
    diffuse = u_dir_light.Diffuse * diff * texture(u_material.Albedo, fs_in.TexUV).rgb;
  } else {
    ambient = u_dir_light.Ambient * DEFAULTCOLOR;
    diffuse = u_dir_light.Diffuse * diff * DEFAULTCOLOR;
  }
  vec3 specular;
  if (u_has_specular_tex > 0) {
    specular = u_dir_light.Specular * spec * texture(u_material.Specular, fs_in.TexUV).r;
  } else {
    specular = u_dir_light.Specular * spec * 0.0;  // no shine    
  }

  return(ambient + diffuse + specular);
}

vec3 CalculatePointLights(PointLight light, vec3 normal, vec3 viewDir){
  vec3 lightDir = normalize(light.Position - fs_in.Pos);
  float diff = max(dot(lightDir, normal), 0.0);

  // specular shading
  float spec;
  if (u_reflection_model.BlinnPhong) {
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), u_material.Shininess * 4);
  } else if (u_reflection_model.Phong) {
    vec3 reflectDir = reflect(-lightDir, normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.), u_material.Shininess);
  } else {
    spec = 0.0;
  }

  float dist = length(light.Position - fs_in.Pos);
  float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));
  vec3 ambient;
  vec3 diffuse;
  if (u_has_albedo_tex > 0) {
    ambient = light.Ambient * texture(u_material.Albedo, fs_in.TexUV).rgb;
    diffuse = light.Diffuse * diff * texture(u_material.Albedo, fs_in.TexUV).rgb;
  } else {
    ambient = light.Ambient * DEFAULTCOLOR;
    diffuse = light.Diffuse * diff * DEFAULTCOLOR;
  }
  vec3 specular;
  if (u_has_specular_tex > 0) {
    specular = light.Specular * spec * texture(u_material.Specular, fs_in.TexUV).r;
  } else {
    specular = light.Specular * spec * 0.0; // no shine
  }
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir){
  vec3 lightDir = normalize(light.Position - fs_in.Pos);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.0);

  // specular shading
  float spec;
  if (u_reflection_model.BlinnPhong) {
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), u_material.Shininess * 4);
  } else if (u_reflection_model.Phong) {
    vec3 reflectDir = reflect(-lightDir, normal);
    spec = pow(max(dot(viewDir, reflectDir), 0.), u_material.Shininess);
  } else {
    spec = 0.0;
  }

  // attenuation
  float dist = length(light.Position - fs_in.Pos);
  float attenuation = 1.0 / (light.Constant + light.Linear * dist + light.Quadratic * (dist * dist));
  // cone of light
  float theta = dot(lightDir, normalize(-light.Direction));
  float epsilon = light.CutOff - light.OuterCutOff;
  float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);
  // combine results
   vec3 ambient;
   vec3 diffuse;
  if (u_has_albedo_tex > 0) {
    ambient = light.Ambient * texture(u_material.Albedo, fs_in.TexUV).rgb;
    diffuse = light.Diffuse * diff * texture(u_material.Albedo, fs_in.TexUV).rgb;
  } else {
    ambient = light.Ambient * DEFAULTCOLOR;
    diffuse = light.Diffuse * diff * DEFAULTCOLOR;
  }
  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;
  

  vec3 specular;
  if (u_has_specular_tex > 0) {
    specular = light.Specular * spec * texture(u_material.Specular, fs_in.TexUV).r;
    specular *= attenuation * intensity;
  } else {
    specular = vec3(0.0);  // no shine
  }
  return (ambient + diffuse + specular);
}

void main() {
  vec3 normal;
  if (u_has_normal_tex > 0) {
    normal = texture(u_material.Normal, fs_in.TexUV).rgb;
    normal = normalize(normal * 2.0 - 1.0);
  } else {
    normal = normalize(fs_in.Norm * 2.0 - 1.0);
  }
  vec3 view_dir = normalize(u_view_pos - fs_in.Pos);
  vec3 result;
  if (u_is_dir_light_on > 0) { result += CalculateDirLight(normal, view_dir); }
  int i = 0;
  for (i; i < u_num_point_lights_in_use; i++) { result += CalculatePointLights(u_point_lights[i], normal, view_dir); }
  for (i = 0; i < u_num_spot_lights_in_use; i++) { result += CalcSpotLight(u_spot_lights[i], normal, view_dir); }
  if (u_has_emission_tex > 0) {
    vec3 emission = texture(u_material.Emission, fs_in.TexUV).rgb;
    result += emission;
  }
  out_Color = vec4(result, 1.0);
}
)";

  UBERSHADER = new OGLShader(UBERSHADER_VERT_CODE.c_str(), UBERSHADER_FRAG_CODE.c_str());
}

OGLShader* Uber::Get() {
  if (!UBERSHADER) {
    Init();
  } else {
    UBERSHADER->Use();
  }
  return UBERSHADER;
}

void Uber::Shutdown() {
  if (UBERSHADER) {
    delete UBERSHADER;
    UBERSHADER = nullptr;
  }
}

bool Uber::IsActive() {
    return UBERSHADER;
}


}
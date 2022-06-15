#include "Uber.h"
#include <string>
namespace AA::InternalShaders {

static OGLShader* UBERSHADER = nullptr;

//todo: optimization - uniform buffers https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL
void Uber::Init() {
  if (UBERSHADER)
    return;

  const std::string UBERSHADER_VERT_CODE = R"(
#version 460 core

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
  } else {  // Not Animating
    vs_out.Pos = (u_model_matrix * vec4(inPos, 1.0)).xyz;
    totalPosition = vec4(inPos, 1.0);
  }

  mat3 normalMatrix = transpose(inverse(mat3(u_model_matrix)));
  vs_out.Norm = normalize(normalMatrix * inNorm);

  mat4 viewMatrix = u_view_matrix * u_model_matrix;
  gl_Position = u_projection_matrix * viewMatrix * totalPosition;
}
)";

  const std::string UBERSHADER_FRAG_CODE = R"(
#version 460 core
layout(location=0)out vec4 out_Color;

in VS_OUT
{
  vec3 Pos;
  vec2 TexUV;
  vec3 Norm;
} fs_in;

struct Material {
  int has_albedo_tex;
  sampler2D Albedo;

  int has_specular_tex;
  sampler2D Specular;

  int has_normal_tex;
  sampler2D Normal;

  int has_emission_tex;
  sampler2D Emission;

  vec3 Tint;
  vec3 Ambient;
  vec3 SpecularColor;
  vec3 EmissionColor;  

  float Shininess;
};

struct DirectionalLight {
  vec3 Direction;
  vec3 Ambient;
  vec3 Diffuse;
  vec3 Specular;
  int Shadows;  // true or false
  float ShadowBiasMin, ShadowBiasMax;
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

uniform int u_is_dir_light_on;
uniform int u_mesh_does_shadow;

uniform Material u_material; // textures 0 to 3
uniform sampler2D u_shadow_map; // texture 4

uniform ReflectionModel u_reflection_model;
uniform vec3 u_view_pos;
uniform mat4 u_light_space_matrix;  

uniform DirectionalLight u_dir_light;

const int MAXPOINTLIGHTS = 24; // if changed, needs to match on light controllers
uniform int u_num_point_lights_in_use;
uniform PointLight u_point_lights[MAXPOINTLIGHTS];

const int MAXSPOTLIGHTS = 12;
uniform SpotLight u_spot_lights[MAXSPOTLIGHTS];
uniform int u_num_spot_lights_in_use;

vec3 CalculateDirLight(vec3 normal, vec3 viewDir);
vec3 CalculatePointLights(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

//const vec3 DEFAULTCOLOR = vec3(1.0, 0.0, 0.0);

void main() {
  vec3 normal;
  if (u_material.has_normal_tex > 0) {
    normal = texture(u_material.Normal, fs_in.TexUV).rgb;
    normal = normalize(normal * 2.0 - 1.0);
  } else {
    //normal = normalize(fs_in.Norm * 2.0 - 1.0);
    normal = fs_in.Norm;
  }
  vec3 view_dir = normalize(u_view_pos - fs_in.Pos);
  vec3 result;
  if (u_is_dir_light_on > 0) { result += CalculateDirLight(normal, view_dir); }
  int i = 0;
  for (i; i < u_num_point_lights_in_use; i++) { result += CalculatePointLights(u_point_lights[i], normal, view_dir); }
  for (i = 0; i < u_num_spot_lights_in_use; i++) { result += CalcSpotLight(u_spot_lights[i], normal, view_dir); }
  if (u_material.has_emission_tex > 0) {
    vec3 emission = texture(u_material.Emission, fs_in.TexUV).rgb;
    result += emission;
  }
  result += u_material.EmissionColor;

  out_Color = vec4(result, 1.0);
}

// Light Calculations
vec3 CalculateDirLight(vec3 normal, vec3 viewDir) {
  vec3 lightDir = normalize(-u_dir_light.Direction);
  // diffuse shading
  float diff = max(dot(normal, lightDir), 0.);
  float dotLightNormal = dot(-u_dir_light.Direction, normal);

  // shadows
  float shadow = 0.0;
  if (u_dir_light.Shadows > 0 && u_mesh_does_shadow > 0) {
    vec4 FragPosLightSpace = u_light_space_matrix * vec4(fs_in.Pos, 1.0);
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(u_shadow_map, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 normal = normalize(fs_in.Norm);
    float bias = max(u_dir_light.ShadowBiasMax * (1.0 - dot(normal, lightDir)), u_dir_light.ShadowBiasMin);
    vec2 texelSize = 1.0 / textureSize(u_shadow_map, 0);
    for(int x = -1; x <= 1; ++x) {
      for(int y = -1; y <= 1; ++y) {
        float pcfDepth = texture(u_shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
      }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }
  }

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
  if (u_material.has_albedo_tex > 0) { 
    ambient = u_dir_light.Ambient * texture(u_material.Albedo, fs_in.TexUV).rgb;
    diffuse = u_dir_light.Diffuse * diff * texture(u_material.Albedo, fs_in.TexUV).rgb;
  } else {
    diffuse = u_dir_light.Diffuse * diff * u_material.Tint;
    ambient = u_dir_light.Ambient * u_material.Ambient;
  }

  vec3 specular = vec3(0.0, 0.0, 0.0);
  if (u_material.has_specular_tex > 0) {
    specular = ((u_dir_light.Specular + u_material.SpecularColor)/2.0) * spec * texture(u_material.Specular, fs_in.TexUV).r;
  }

  return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalculatePointLights(PointLight light, vec3 normal, vec3 viewDir) {
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
  if (u_material.has_albedo_tex > 0) {
    ambient = light.Ambient * texture(u_material.Albedo, fs_in.TexUV).rgb;
    diffuse = light.Diffuse * diff * texture(u_material.Albedo, fs_in.TexUV).rgb;
  }

//else {
//    ambient = light.Ambient * DEFAULTCOLOR;
//    diffuse = light.Diffuse * diff * DEFAULTCOLOR;
//  }

  vec3 specular;
  if (u_material.has_specular_tex > 0) {
    specular = light.Specular * spec * texture(u_material.Specular, fs_in.TexUV).r;
  } else {
    specular = light.Specular * spec * 0.0; // no shine
  }
  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;
  return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
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
  if (u_material.has_albedo_tex > 0) {
    ambient = light.Ambient * texture(u_material.Albedo, fs_in.TexUV).rgb;
    diffuse = light.Diffuse * diff * texture(u_material.Albedo, fs_in.TexUV).rgb;
  }

// else {
//    ambient = light.Ambient * DEFAULTCOLOR;
//    diffuse = light.Diffuse * diff * DEFAULTCOLOR;
//  }
  ambient *= attenuation * intensity;
  diffuse *= attenuation * intensity;

  vec3 specular;
  if (u_material.has_specular_tex > 0) {
    specular = light.Specular * spec * texture(u_material.Specular, fs_in.TexUV).r;
    specular *= attenuation * intensity;
  } else {
    specular = vec3(0.0);  // no shine
  }
  return (ambient + diffuse + specular);
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
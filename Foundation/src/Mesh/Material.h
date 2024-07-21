#pragma once
namespace AA {

/// <summary>
/// Structure of our supported Materials
/// </summary>
struct Material {
public:
  // Kd: specifies diffuse color, which typically contributes most of the 
  // color to an object [see Wikipedia entry for Diffuse Reflection]. will get modified by a 
  // colored texture map specified in the map_Kd statement
  glm::vec3 Diffuse{};

  // Ka: specifies ambient color, to account for light that is scattered
  // about the entire scene[see Wikipedia entry for Phong Reflection Model] using values 
  // between 0 and 1 for the RGB components.
  glm::vec3 Ambient{};

  // Ks: specifies specular color, the color seen where the surface
  // is shiny and mirror-like [see Wikipedia entry for Specular Reflection].
  glm::vec3 SpecularColor{};

  // color of the emission
  glm::vec3 Emission{};

  // float SpecularHightlights{};  // Ns: defines the focus of specular highlights in the material. Ns values normally range from 0 to 1000, with a high value resulting in a tight, concentrated highlight.

  // float OpticalDensity{};       // Ni: defines the optical density (aka index of refraction) in the current material. The values can range from 0.001 to 10. A value of 1.0 means that light does not bend as it passes through an object.

  // float Dissolve{};             // d: specifies a factor for dissolve, how much this material dissolves into the background. A factor of 1.0 is fully opaque. A factor of 0.0 is completely transparent.

  // int IlluminationModel{};      //  specifies an illumination model, using a numeric value. See Notes below for more detail on the illum keyword. The value 0 represents the simplest illumination model, relying on the Kd for the material modified by a texture map specified in a map_Kd statement if present. The compilers of this resource believe that the choice of illumination model is irrelevant for 3D printing use and is ignored on import by some software applications. For example, the MTL Loader in the threejs Javascript library appears to ignore illum statements

  // texture maps to blend with  - data not saved, see TextureInfo.h
};

}
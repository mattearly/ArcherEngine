# AncientArcher2

AncientArcher is a C++ library for game development and simulations.

## Project
 
 - AAEngine

   Context Handled: 
    - 3D/2D Renderering
    - Model & Texture Loaders with optimized resource management 
    - Point, Directional, and Spot Lights
    - Spacial Transformations
    - Function Based Client Core Loop
    - Skeletal Animations
    - Physics (testing)
    - Input Controls
    - Windowing
    - Cameras
    - Phong GLSL Shaders
    - Cubemap Skyboxes
    - 3D Sound

## Documentation

  - Generate via Doxygen
  - See header [Interface.h](AAEngine/include/AncientArcher/Interface.h)

## Compile

Recommended: 
 - OS: Windows 10.
 - IDE: MSVC17, x64. Visual Studio 2022 Community Edition.

*While AncientArcher code is generally designed to be crossplatform, at this stage the build system is handled via Visual Studio Solution file. Converting it to CMake will be done at some point. If you are just using some of the code and not the whole project it should work the same on any OS.*

## Dependencies

glm, assimp, entt, glfw3, glad, OpenAL-Soft, sndfile, imgui, physx, stb, curl

*managed by vcpkg manifest*

## Tests and Samples

- Tests via MicrosoftCPPUnitTests: To Run the tests fully, you must download the RuntimeFiles resources (i.e. run DownloadTestResources and unzip to the RuntimeFiles/ dir).

## Sample usage code

```cpp
#include <AAEngine/Interface.h>

AA::Interface instance;

int main(int argc, char** argv) {
  instance.Init();
  /* 
   your code here -> implement camera, lights, models, sounds, 
   input handling, and their logic at will 
  */
  // instance.Add...
  return instance.Run();
}
```
<img src="docs/AncientArcher.png" height="56px" align="right">

# AncientArcher

AncientArcher is a C++ library for game development and simulations.

### [Public Space Docs Webpage](https://mattearly.github.io/AncientArcher/index.html)

<img src="docs/walking_anim_demo.gif" height="256px" align="right">

Context Handled: 
 - 3D/2D Renderering
 - Model & Texture Loaders with optimized resource management 
 - Point, Directional, and Spot Lights
 - Spacial Transformations
 - Function Based Client Core Loop
 - Skeletal Animations (testing)
 - Physics(testing)
 - Input Controls
 - Windowing
 - Cameras
 - GLSL Shaders
 - Cubemap Skyboxes
 - 3D Sound

### Compile

Recommended: 
 - OS: Windows 10.
 - IDE: MSVC16, x64. Visual Studio 2019 Community Edition.

*While AncientArcher code is generally designed to be crossplatform, at this stage the build system is handled via Visual Studio Solution file. Converting it to CMake will be done at some point. If you are just using some of the code and not the whole project it should work the same on any OS.*

### Dependencies

glm, assimp, glfw3, glad, OpenAL-Soft, sndfile, imgui

*managed by vcpkg manifest*

### Tests and Samples

- CoreTests: base case tests 
- ExampleProject: fpp with walk controls

Link as you would any other C++ library, and then you can begin using it in your code.

### Sample usage code

```cpp
#include <AncientArcher/AncientArcher.h>

AA::AncientArcher instance;

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
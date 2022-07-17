# ArcherEngine

*ArcherEngine is a Multi-Media C++ library for setting up and interfacing with 3D enviorments. All of the functionality is accessed through C++ functions.*

## Use Context

- Windowing
- 3D Scene Loader and transform controls
- OpenGL Render Pipeline
- Basic Texturing, Color Emissions
- Directional, Point, and Spot Lights
- Skeletal Animation Loader & Player
- Sunlight Shadows
- Cameras
- 3D SoundEffects
- Ambient/Music background music
- Run Time scripts C++ Lambdas

## The Docs

- Generate at wil via Doxygen
- See [Interface.h](AAEngine/include/AAEngine/Interface.h) Header for User Access Functions

## Build & Run

Requirements
 - OS: Windows 10
 - IDE: [Visual Studio 2022](https://visualstudio.microsoft.com/vs/), MSVC17, x64. 

### Open Source Libraries Used

assimp, curl, glfw3, glad, glm, imgui, physx, OpenAL-Soft, sndfile, stb

- libraries are automatically managed by [vcpkg](https://github.com/microsoft/vcpkg), using a project manifest, which builds them locally for your  All you should have to do is setup vcpkg and when you build these will all build as required.

## Tests and Samples

### MicrosoftCPPUnitTests

These tests rely on Multimedia Files that are not on the GitHub repo. 
Download & Move the Multimedia Files before running the tests.
[src:DownloadTestResources.cpp](https://github.com/mattearly/ArcherEngine/blob/main/DownloadTestResources/DownloadTestResources.cpp)
 1. Build & Run the DownloadTestResources project. 
 2. Unzip the files it downloads and put them in a folder called `RuntimeFiles/` on the root directory of this repo.
 3. Tests should now run. Access from the Tests menu in Visual Studio `Tests->Run All Tests (Ctrl+R, A)`.

## Example Code

```cpp
#include <AAEngine/Interface.h>
AA::Interface instance; // global so you can access it in lambda scripting
int main(int argc, char** argv) {
  WindowOptions opts;
  instance.Init(opts);  // Initializes Required Hardware Access
  /* 
   * add camera, lights, models, sounds, animations, controls, scripts, etc
  */
  return instance.Run();  // starts the simulation and stays running until Shutdown() or Window.Close()
}
```


## Development

Currently being developed and managed by this repo owner.

Development Schedule (Monthly Release):
- The `main` should only be updated via pull requests from `dev`. Done once a month, with an accompanied release.
- The `dev` branch should only be updated via Pull Requests.
- As for `other-branches`, create them from starting point `dev` to do work, and merge them into `dev` when complete. 
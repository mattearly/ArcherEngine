# ArcherEngine

*ArcherEngine is a Multi-Media C++ library for setting up and interfacing with 3-D enviorments.*

## Use Context

- 3-D Model Loader and transform controls
- Texturing & Emissions. 
- Directional, Point, and Spot Lights.
- Models with Skeletal Animations and some Controls.
- Cameras Settings and Controls.
- 3-D SoundEffects.
- Background Soundtracks.
- Customize Run time scripts with C++ Lambdas.

## The Docs

  - Generate at wil via Doxygen
  - See [Interface.h](AAEngine/include/AAEngine/Interface.h) Header for User Access Functions

## Build & Run

Requirements
 - OS: Windows 10
 - IDE: [Visual Studio 2022](https://visualstudio.microsoft.com/vs/), MSVC17, x64. 

### Open Source Libraries Used

assimp, curl, entt, glfw3, glad, glm, imgui, physx, OpenAL-Soft, sndfile, stb

- libraries are automatically managed by [vcpkg](https://github.com/microsoft/vcpkg), using a project manifest, which builds them locally for your  All you should have to do is setup vcpkg and when you build these will all build as required.

## Tests and Samples

### MicrosoftCPPUnitTests

To fully run the tests, you must:
 - Build & Run the DownloadTestResources project. 
 - Unzip the files it downloads and put them on the root of `RuntimeFiles/`
 - Go to `Tests->Run All Tests (Ctrl+R, A)`.

## Example Code

```cpp
#include <AAEngine/Interface.h>
AA::Interface instance; // global so you can access it in lambda scripting
int main(int argc, char** argv) {
  WindowOptions opts;
  instance.Init(opts);  // Initializes Required Hardware Access
  /* 
   *your code here* 
   *add camera, lights, models, sounds, animations, controls, etc*
   *look at the various Add/Remove functions and their signatures.*
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
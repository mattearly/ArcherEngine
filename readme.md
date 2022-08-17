# ArcherEngine

*Archer Engine is a Multimedia C++ library for setting up and interfacing with 3D environments. All of the functionality is accessed through C++ functions.*

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

## Build & Run

### (docs/readme.md)[docs/readme.md]

### Unit Tests

TODO

### Examples

TODO

## Example Code

```cpp
#include <AAEngine/Interface.h>
AA::Interface instance; // global so you can access it in lambda scripting
int main(int argc, char** argv) {
  WindowOptions opts;
  instance.Init(opts);  // Initializes Required Hardware Access
  /* 
   * add camera, lights, models, sounds, animations, controls, lambda scripts, etc
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

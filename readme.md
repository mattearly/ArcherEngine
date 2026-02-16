# ArcherEngine

*Archer Engine is a Multimedia C++ library for setting up and interfacing with 3D environments.*

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
- Run Time C++ Scripting

## Build & Run

### [doc/readme.md](doc/readme.md)

### Unit Tests

TODO

### Examples

| SYNTAX | DESCRIPTION |
| ------ | ----------- |
| ../build/samples/editor/editor_example.exe | model importer and flying cam |

## Example Code

```cpp
#include <ArcherEngine/Interface.h>
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

[Original project](https://github.com/mattearly/AncientArcher). 

This new version should build on any OS.

Might occasianally apply fixes, update things, or merge things from others. There is no specific schedule to the project as this is considered a random old side project to me now.

### Branch Rules

- `docs` code and website documentation.
- `main` should only be updated via pull requests from `dev`.
- `dev` branch should only be updated via Pull Requests from `other-branches`.
- As for `other-branches`, create them from  `dev` to do work, and merge them into `dev` when complete.
# ArcherEngine

*Archer Engine is a Multimedia C++ library for setting up and interfacing with 3D environments. All of the functionality is accessed through C++ functions in archer lib*

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

### [doc/readme.md](doc/readme.md)

### Unit Tests

TODO

### Examples

| SYNTAX | DESCRIPTION |
| ------ | ----------- |
| ../build/samples/editor/editor_example.exe | Example Game Engine Front End Editor Starting Point |

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

Currently being developed as a continuation of [the original project](https://github.com/mattearly/AncientArcher). This new version should build on any OS. Making a game engine library is a huge task and I could use all the help I can get.

### Development Schedule

 Currently the aim is to put out monthly updates. You can find these in video form on the Code, Tech, and Tutorials YouTube Channel.

### Branch Rules

- `main` should only be updated via pull requests from `dev`. Done once a month, with an accompanied release.
- `dev` branch should only be updated via Pull Requests from `other-branches`
- As for `other-branches`, create them from starting point `dev` to do work, and merge them into `dev` when complete. 

These rules are somewhat loose but we need some structure to allow an understanding of how others might be trying to work on things.

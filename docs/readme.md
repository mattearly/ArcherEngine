# ArcherEngine Guide

## Summary

*The static archer lib will need to be linked and included into your C++ project.*

1. Build the ArcherEngine project(address needed dependecies if it fails, and try again).
2. Link the `archer.(lib/a)` file to your project.
3. Include ArcherEngine public headers in your project.

## Working with archer library

Some (hopefully most) of the public function descriptions can be found embedded in the header files, mainly the public ones. You can:

- Generate these into various readable forms via Doxygen
- Go right to [Interface.h](AAEngine/include/AAEngine/Interface.h) Header to get a basic understanding of publicly exposed functionality.

## Building for your OS

- LINUX
  - External Libraries will be required to build Archer.
  - Linux Mint required libs:
    - `sudo apt-get install libimgui-dev libglfw3-dev libglm-dev libopengl-dev libsndfile-dev libopenal-dev libcurl-dev libassimp-dev libglew-dev`
  - build commands:
    - `chmod +x linux_build.sh`
    - `./linux_build.sh`

- MACOS
  - TODO (should be prettys similar to linux, however you'll probably need homebrew and to adjust commands as necessary.)

- WINDOWS
  - requires vcpkg to handle libs
    - see [https://github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)
    - ArcherEngine Project should pull down and build the required libraries via the vcpkg.json manifest once vcpkg is set up and first attempt at cmake configuration is initiated
  - use cmake to prepare your build from the project directory and then build with desired compiler.
    - defer to cmake instructions, you can use their cmake exe or call cmake from powershell or cmd if it is set up to be found on your system.

Additional Build Notes:

- By default the examples will be built.
  - editor exe: You can run the editor example by going to `/examples/editor` in your build directory and launching the editor_example.exe or using `./editor_example` from terminal. In this editor, you can fly around with wasd, move down with c, move up with spacebar, look with the mouse, toggle fpp control with right click. You can also do some basic loading of files. This is not a complete editor or project, just a sample start of one that still needs a lot of work.

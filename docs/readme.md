# ArcherEngine Guide

archer lib will need to be linked to your C++ project to use. This should be pretty common practice for an experienced C++ user. Build it, link it, put the public headers in your include space, and you can use the lib.

## Building for your OS

### LINUX

External Libraries will be required to build Archer.

#### Linux Mint:

##### required libs:
```bash
sudo apt-get install libimgui-dev libglfw3-dev libglm-dev libopengl-dev libsndfile-dev libopenal-dev libcurl-dev libassimp-dev libglew-dev libopenal-dev
```

##### build commands:
```bash
chmod +x linux_build.sh
./linux_build.sh
```

By default the examples will be built. You can run the fpp_demo by going to build/examples/fpp and typing ./fpp_demo
the demo does not have any models loaded by default, so you will see a black screen. Drag a model file on the screen to load it.
You can fly around with wasd, move down with c, move up with spacebar, look with the mouse, toggle fpp control with right click.
That is all the demo does for now.

### MACOS

TODO

### WINDOWS

#### requires vcpkg to handle libs

see [https://github.com/microsoft/vcpkg](https://github.com/microsoft/vcpkg)
should work via the vcpkg.json manifest once vcpkg is set up

use cmake to prepare your build.

## Library Function Definitions

- Generate via Doxygen
- See [Interface.h](AAEngine/include/AAEngine/Interface.h) Header for core user access functions.
- Old ones (todo - new version): [Old Docs Webpage](https://mattearly.github.io/AncientArcher/index.html)

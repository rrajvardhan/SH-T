# Systemically Handle ! Things

Or simply,  **SH!T** - attempt to build a 2D game engine prototype using SDL2 and C++.

## Overview

* ECS-based game engine written in C++
* Uses SDL2 for input handling, rendering, and windowing
* Lua scripting via Sol2 for gameplay logic
* JSON-based scene and entity serialization
* Modular managers for input, audio, textures, fonts, and more

## A Quick Look
https://github.com/user-attachments/assets/897cec8d-a5f1-4f9d-b024-591b27a37fa1

*The sickle-wielding pixel warrior? Not mine. Credit goes to [Immortal_burrito](https://immortal-burrito.itch.io) for the [Sickle Warrior asset pack](https://immortal-burrito.itch.io/sickle-warrior).*


## Build Instructions

### Prerequisites

You’ll need the following dependencies installed:

* SDL2
* SDL2\_image
* SDL2\_ttf
* SDL2\_mixer
* Lua
* CMake
* nlohmann-json
* A C++17 compatible compiler

### Linux

Install dependencies using your preferred package manager (`pacman`, `apt`, `dnf`, etc).

> Package names may vary slightly depending on your distro. For example, some systems use libsdl2-dev instead of sdl2.

```bash
sudo pacman -S sdl2 sdl2_image sdl2_ttf sdl2_mixer lua cmake make nlohmann-json
```

Then clone, build, and run:

```bash
git clone https://github.com/rrajvardhan/SH-T
cd SH-T
mkdir build && cd build
cmake ..
make
./main
```

> If CMake can’t find a dependency, you may need to manually specify include/library paths or adjust your environment variables.

### Windows (Untested, but theoretically possible)

It should work. You’ll need to install the same dependencies, and then ensure CMake can find them.

> *If you test this on Windows, feel free to submit a PR with working build instructions!*

Install Visual Studio 2026 with Desktop development with C++, CMake, Git, and vcpkg.

Clone and build:

git clone https://github.com/rrajvardhan/SH-T
cd SH-T
mkdir build
cd build

cmake .. -G "Visual Studio 18 2026" -A x64 -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake

cmake --build . --config Release

Run:

.\Release\main.exe

vcpkg automatically installs the dependencies specified in vcpkg.json.




## Try the Example

You can try the exact scene shown above.

After building, copy the example assets into your `build/` directory and run the engine:

```bash
# From inside the build/ directory
cp -r ../example/* .
./main
```

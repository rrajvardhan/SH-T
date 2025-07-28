# Systemically Handle ! Things

Or simply,  **SH!T**  — attempt to build a 2D game engine prototype using SDL2 and C++.

## Overview

* ECS-based game engine written in C++
* Uses SDL2 for input handling, rendering, and windowing
* Lua scripting via Sol2 for gameplay logic
* JSON-based scene and entity serialization
* Modular managers for input, audio, textures, fonts, and more

## Build Instructions (Linux only)

### Prerequisites

You’ll need the following dependencies installed:

* SDL2
* SDL2\_image
* SDL2\_ttf
* SDL2\_mixer
* Lua
* CMake
* A C++17 compatible compiler

Install on Arch Linux:

```bash
sudo pacman -S sdl2 sdl2_image sdl2_ttf sdl2_mixer lua cmake make gcc
```

### Build

```bash
git clone https://github.com/rrajvardhan/SH-T
cd SH-T
mkdir build && cd build
cmake ..
make
./main
```

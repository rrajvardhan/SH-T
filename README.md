# Systemically Handle ! Things

Or simply,  **SH!T**  — attempt to build a 2D game engine prototype using SDL2 and C++.

## Overview

* ECS-based game engine written in C++
* Uses SDL2 for input handling, rendering, and windowing
* Lua scripting via Sol2 for gameplay logic
* JSON-based scene and entity serialization
* Modular managers for input, audio, textures, fonts, and more

## A Quick Look
https://github.com/user-attachments/assets/897cec8d-a5f1-4f9d-b024-591b27a37fa1

*The sickle-wielding pixel warrior? Not mine. Credit goes to [Immortal_burrito](https://immortal-burrito.itch.io) for the [Sickle Warrior asset pack](https://immortal-burrito.itch.io/sickle-warrior).*


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

### Windows (Untested)

This should work on Windows. You'll need the same dependencies (SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, Lua) and ensure CMake can find them.

> *If you test this on Windows, feel free to submit a PR with working build instructions!*

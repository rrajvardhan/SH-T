#pragma once

#include "AudioManager.hpp"
#include "FontManager.hpp"
#include "InputManager.hpp"
#include "TextureManager.hpp"
#include "Timer.hpp"

struct EngineContext {
  InputManager *input = nullptr;
  TextureManager *texture = nullptr;
  FontManager *font = nullptr;
  AudioManager *audio = nullptr;
  Timer *timer = nullptr;

  int screenWidth = 0;
  int screenHeight = 0;
};

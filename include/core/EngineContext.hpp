#pragma once

#include "AudioManager.hpp"
#include "Configs.hpp"
#include "FontManager.hpp"
#include "Graphics.hpp"
#include "InputManager.hpp"
#include "TextureManager.hpp"
#include "Timer.hpp"

struct EngineContext {
  Graphics *graphics = nullptr;
  InputManager *input = nullptr;
  TextureManager *texture = nullptr;
  FontManager *font = nullptr;
  AudioManager *audio = nullptr;
  Timer *timer = nullptr;

  EngineConfig *engineConfig = nullptr;
};

#pragma once

#include "AudioManager.hpp"
#include "FontManager.hpp"
#include "Graphics.hpp"
#include "InputManager.hpp"
#include "ServiceContext.hpp"
#include "TextureManager.hpp"
#include "Timer.hpp"
#include <SDL2/SDL_events.h>
#include <memory>

class Services
{
public:
  Services();
  ~Services();

  bool init(const EngineConfig& config);

  Graphics&       getGraphics() { return _graphics; }
  Timer&          getTimer() { return _timer; }
  TextureManager& getTextureManager() { return *_textureManager; }
  FontManager&    getFontManager() { return *_fontManager; }
  AudioManager&   getAudioManager() { return *_audioManager; }

  ServiceContext getContext();

private:
  EngineConfig _config;

  Graphics                        _graphics;
  InputManager                    _input;
  Timer                           _timer;
  std::unique_ptr<TextureManager> _textureManager;
  std::unique_ptr<FontManager>    _fontManager;
  std::unique_ptr<AudioManager>   _audioManager;
};

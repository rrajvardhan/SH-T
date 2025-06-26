#pragma once

#include "AudioManager.hpp"
#include "EngineContext.hpp"
#include "FontManager.hpp"
#include "Graphics.hpp"
#include "InputManager.hpp"
#include "TextureManager.hpp"
#include "Timer.hpp"
#include <SDL2/SDL_events.h>
#include <memory>

class Engine {
public:
  static Engine *getInstance();
  static void release();

  bool init(const EngineConfig &config);
  void beginFrame();
  void endFrame();
  bool isRunning() const { return !_quit; }

  Graphics &getGraphics() { return _graphics; }
  Timer &getTimer() { return _timer; }
  TextureManager &getTextureManager() { return *_textureManager; }
  FontManager &getFontManager() { return *_fontManager; }
  AudioManager &getAudioManager() { return *_audioManager; }

  EngineContext getContext();

private:
  Engine();
  ~Engine();

  EngineConfig _config;

  static Engine *_instance;
  bool _quit;
  SDL_Event _event;

  Graphics _graphics;
  InputManager _input;
  Timer _timer;

  std::unique_ptr<TextureManager> _textureManager;
  std::unique_ptr<FontManager> _fontManager;
  std::unique_ptr<AudioManager> _audioManager;
};

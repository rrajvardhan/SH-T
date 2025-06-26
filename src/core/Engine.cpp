#include "Engine.hpp"
#include "FontManager.hpp"
#include "Graphics.hpp"
#include "Log.hpp"
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_video.h>
#include <memory>

Engine *Engine::_instance = nullptr;

Engine *Engine::getInstance() {

  if (_instance == nullptr)
    _instance = new Engine();

  return _instance;
}

void Engine::release() {

  delete _instance;
  _instance = nullptr;
}

Engine::Engine() : _quit(false) {}

Engine::~Engine() {}

bool Engine::init(const EngineConfig &config) {
  _config = config;

  if (!_graphics.init(config.renderer)) {
    LOG_ERROR("[Graphics] Graphics failed to initialize.");
    return false;
  }

  _textureManager = std::make_unique<TextureManager>(_graphics.getRenderer());
  _fontManager = std::make_unique<FontManager>(_graphics.getRenderer());
  _audioManager = std::make_unique<AudioManager>();

  _quit = false;
  return true;
}

EngineContext Engine::getContext() {
  EngineContext ctx = {
      .input = &_input,
      .texture = _textureManager.get(),
      .font = _fontManager.get(),
      .audio = _audioManager.get(),
      .timer = &_timer,

      .screenWidth = _config.renderer.width,
      .screenHeight = _config.renderer.height,
  };
  return ctx;
}

void Engine::beginFrame() {

  _timer.update();
  while (SDL_PollEvent(&_event)) {
    if (_event.type == SDL_QUIT) {
      _quit = true;
    }
  }

  _input.update();
  _graphics.clear();
}

void Engine::endFrame() {
  _graphics.present();
  _input.updatePrev();

  if (_config.targetFPS > 0) {
    float dt = _timer.getDeltaTime();
    float frameTime = 1.0f / _config.targetFPS;

    if (dt < frameTime) {
      float delay = (frameTime - dt) * 1000.0f;
      SDL_Delay((Uint32)delay);
      dt = frameTime;
    }

    // LOG_DEBUG("deltatime: ", dt);
    // LOG_DEBUG("FPS: ", 1.0f / dt);
  }

  _timer.reset();
}

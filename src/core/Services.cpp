#include "Services.hpp"
#include "FontManager.hpp"
#include "Graphics.hpp"
#include "Log.hpp"
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_video.h>
#include <memory>

Services::Services()
{
}

Services::~Services()
{
}

bool
Services::init(const EngineConfig& config)
{
  _config = config;

  if (!_graphics.init(config.renderer))
  {
    LOG_ERROR("[Graphics] Graphics failed to initialize.");
    return false;
  }

  _textureManager = std::make_unique<TextureManager>(_graphics.getRenderer());
  _fontManager    = std::make_unique<FontManager>(_graphics.getRenderer());
  _audioManager   = std::make_unique<AudioManager>();

  return true;
}

ServiceContext
Services::getContext()
{
  ServiceContext ctx = {
    .graphics = &_graphics,
    .input    = &_input,
    .texture  = _textureManager.get(),
    .font     = _fontManager.get(),
    .audio    = _audioManager.get(),
    .timer    = &_timer,

    .engineConfig = &_config,
  };
  return ctx;
}

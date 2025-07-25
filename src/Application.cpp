#include "Application.hpp"
#include "Log.hpp"
#include "ServiceContext.hpp"
#include "World.hpp"
#include <SDL2/SDL_scancode.h>
#include <memory>

bool
Application::init()
{
  return init(_cfg);
}

bool
Application::init(EngineConfig& cfg)
{

  if (!_services.init(cfg))
  {
    LOG_ERROR("[Application] Services failed to initialize.");
    return false;
  }
  _cfg = cfg;
  _ctx = _services.getContext();

  _world = std::make_unique<World>(_ctx);
  if (!_world->init())
  {
    LOG_ERROR("[World] failed to initialize.");
    return false;
  }

  _world->getCamera().setViewport(_ctx.graphics->getScreenWidth(),
                                  _ctx.graphics->getScreenHeight());

  _editor = std::make_unique<Editor>(*_world, _ctx);

  _running = true;
  LOG_SUCCESS("[Application] Initialized.");
  return true;
}

void
Application::run()
{
  while (_running)
  {

    _ctx.timer->update();

    pollEvents();
    _ctx.input->update();
    _ctx.graphics->clear();

    processInput();
    update();
    render();

    _ctx.graphics->present();
    _ctx.input->updatePrev();
    _ctx.timer->reset();

    capFPS();
  }
}

void
Application::pollEvents()
{
  while (SDL_PollEvent(&_event))
  {
    _editor->events(&_event);
    switch (_event.type)
    {
    case SDL_QUIT:
      _running = false;
      break;

    case SDL_WINDOWEVENT:
      if (_event.window.event == SDL_WINDOWEVENT_RESIZED
          || _event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
      {
        _width  = _event.window.data1;
        _height = _event.window.data2;

        _cfg.renderer.width  = _width;
        _cfg.renderer.height = _height;

        _ctx.graphics->onResize(_width, _height);
        _world->getCamera().setViewport(_width, _height);
      }
      break;
    }
  }
}

void
Application::capFPS()
{

  float dt        = _ctx.timer->getDeltaTime();
  float frameTime = 1.0f / _cfg.targetFPS;

  if (_cfg.targetFPS > 0 && dt < frameTime)
  {
    float delay = (frameTime - dt) * 1000.0f;
    SDL_Delay((Uint32) delay);
    dt = frameTime;
  }
}

void
Application::processInput()
{
  if (_ctx.input->keyDown(SDL_SCANCODE_F12))
  {
    _running = false;
  }
}

void
Application::update()
{
  _editor->update();

  if (!_editor->isActive())
    _world->update();
}

void
Application::render()
{
  _editor->render();
}

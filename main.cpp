#include "Engine.hpp"
#include "EngineContext.hpp"
#include "Game.hpp"
#include "Log.hpp"

int main() {
  Engine *engine = Engine::getInstance();

  RendererConfig rendererCfg = {
      .title = "Engine",
      .x = SDL_WINDOWPOS_CENTERED,
      .y = SDL_WINDOWPOS_CENTERED,
      .width = 1600,
      .height = 900,
      .windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
      .rendererFlags = SDL_RENDERER_ACCELERATED,
  };

  EngineConfig cfg = {
      .renderer = rendererCfg,
      .targetFPS = 60,
  };

  if (!engine->init(cfg)) {
    LOG_ERROR("Engine failed to initialize.");
    return -1;
  }

  EngineContext ctx = engine->getContext();

  Game game(ctx);
  game.init();

  while (engine->isRunning()) {
    engine->beginFrame();

    if (game.isRunning()) {
      game.update();
      game.render();
    }

    engine->endFrame();
  }

  return 0;
}

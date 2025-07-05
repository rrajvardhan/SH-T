#include "Application.hpp"
#include "Log.hpp"

int
main()
{
  RendererConfig rendererCfg = {
    .title         = "Services",
    .x             = SDL_WINDOWPOS_CENTERED,
    .y             = SDL_WINDOWPOS_CENTERED,
    .width         = 1600,
    .height        = 900,
    .windowFlags   = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
    .rendererFlags = SDL_RENDERER_ACCELERATED,
  };

  EngineConfig cfg = {
    .renderer  = rendererCfg,
    .targetFPS = 60,
  };

  Application app;
  if (!app.init(cfg))
  {
    LOG_ERROR("app failed to initialize.");
    return -1;
  }
  app.run();

  return 0;
}

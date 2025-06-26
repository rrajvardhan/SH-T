#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

struct RendererConfig {
  const char *title = "Untitled";
  int x = SDL_WINDOWPOS_CENTERED;
  int y = SDL_WINDOWPOS_CENTERED;
  int width = 800;
  int height = 600;
  int windowFlags = SDL_WINDOW_SHOWN;
  int rendererFlags = SDL_RENDERER_ACCELERATED;
};

struct EngineConfig {
  RendererConfig renderer;
  int targetFPS = 60;
};

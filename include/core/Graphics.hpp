#pragma once

#include "Configs.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Graphics {
public:
  Graphics();
  ~Graphics();

  bool init(const RendererConfig &config);
  void render();
  void clear();
  void present();

  SDL_Renderer *getRenderer() const { return _renderer; }
  SDL_Window *getWindow() const { return _window; }

  void setRenderColor(int r, int g, int b, int a) { SDL_SetRenderDrawColor(_renderer, r, g, b, a); }

  int getScreenWidth() const { return _screenWidth; }
  int getScreenHeight() const { return _screenHeight; }

private:
  SDL_Window *_window = nullptr;
  SDL_Renderer *_renderer = nullptr;

  int _screenWidth = 800;
  int _screenHeight = 450;
};

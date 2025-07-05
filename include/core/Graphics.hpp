#pragma once

#include "Configs.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Graphics
{
public:
  Graphics();
  ~Graphics();

  bool init(const RendererConfig& config);
  void onResize(int width, int height);
  void render();
  void clear();
  void present();

  SDL_Renderer* getRenderer() { return _renderer; }
  SDL_Window*   getWindow() { return _window; }

  int getScreenWidth() const { return _screenWidth; }
  int getScreenHeight() const { return _screenHeight; }

  void setDrawColor(SDL_Color color)
  {
    SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
  }

  void drawRectOutline(int x, int y, int w, int h, SDL_Color color);
  void drawFilledRect(int x, int y, int w, int h, SDL_Color color);
  void drawLine(int x1, int y1, int x2, int y2, SDL_Color color);
  void drawPoint(int x, int y, SDL_Color color);
  void drawCircle(int cx, int cy, int radius, SDL_Color color);
  void drawFilledCircle(int cx, int cy, int radius, SDL_Color color);

private:
  SDL_Window*   _window   = nullptr;
  SDL_Renderer* _renderer = nullptr;

  int _screenWidth  = 800;
  int _screenHeight = 450;
};

#pragma once

#include "Configs.hpp"
#include "Log.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <cctype>
#include <string>

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

  void setBlendMode(const std::string& mode)
  {
    SDL_BlendMode blendMode = SDL_BLENDMODE_INVALID;

    if (mode == "none")
      blendMode = SDL_BLENDMODE_NONE;
    else if (mode == "blend")
      blendMode = SDL_BLENDMODE_BLEND;
    else if (mode == "add")
      blendMode = SDL_BLENDMODE_ADD;
    else if (mode == "mod")
      blendMode = SDL_BLENDMODE_MOD;
    else if (mode == "mul")
      blendMode = SDL_BLENDMODE_MUL;

    if (blendMode == SDL_BLENDMODE_INVALID)
    {
      LOG_ERROR("[Graphics] Invalid SDL Blend mode.");
      return;
    }

    SDL_SetRenderDrawBlendMode(_renderer, blendMode);
  }

  void setTextureBlendMode(SDL_Texture* tex, const std::string& mode)
  {
    SDL_BlendMode blendMode = SDL_BLENDMODE_INVALID;

    if (mode == "none")
      blendMode = SDL_BLENDMODE_NONE;
    else if (mode == "blend")
      blendMode = SDL_BLENDMODE_BLEND;
    else if (mode == "add")
      blendMode = SDL_BLENDMODE_ADD;
    else if (mode == "mod")
      blendMode = SDL_BLENDMODE_MOD;
    else if (mode == "mul")
      blendMode = SDL_BLENDMODE_MUL;

    if (blendMode == SDL_BLENDMODE_INVALID)
    {
      LOG_ERROR("[Graphics] Invalid SDL Blend mode.");
      return;
    }

    SDL_SetTextureBlendMode(tex, blendMode);
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

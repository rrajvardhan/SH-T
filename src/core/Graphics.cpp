#include "Graphics.hpp"
#include "Log.hpp"

Graphics::Graphics() : _window(nullptr), _renderer(nullptr) {}

Graphics::~Graphics() {
  SDL_DestroyRenderer(_renderer);
  _renderer = nullptr;
  SDL_DestroyWindow(_window);
  _window = nullptr;

  SDL_Quit();
}

bool Graphics::init(const RendererConfig &config) {

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {

    LOG_ERROR("[Graphics] SDL initialization failed: ", SDL_GetError());
    return false;
  }

  _window = SDL_CreateWindow(config.title, config.x, config.y, config.width, config.height, config.windowFlags);

  if (!_window) {
    LOG_ERROR("[Graphics] Failed to create window: ", SDL_GetError());
    return false;
  }

  _renderer = SDL_CreateRenderer(_window, -1, config.rendererFlags);
  if (!_renderer) {
    LOG_ERROR("[Graphics] Failed to create renderer: ", SDL_GetError());
    return false;
  }

  _screenWidth = config.width;
  _screenHeight = config.height;
  return true;
}

void Graphics::clear() {
  SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
  SDL_RenderClear(_renderer);
}
void Graphics::present() { SDL_RenderPresent(_renderer); }

void Graphics::render() {
  clear();
  present();
}

void Graphics::drawFilledRect(int x, int y, int w, int h, SDL_Color color) {
  SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
  SDL_Rect rect = {x, y, w, h};
  SDL_RenderFillRect(_renderer, &rect);
}

void Graphics::drawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
  SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
  SDL_RenderDrawLine(_renderer, x1, y1, x2, y2);
}

void Graphics::drawPoint(int x, int y, SDL_Color color) {
  SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
  SDL_RenderDrawPoint(_renderer, x, y);
}

// Midpoint circle approximation
void Graphics::drawCircle(int cx, int cy, int radius, SDL_Color color) {
  SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);

  int x = radius;
  int y = 0;
  int err = 0;

  while (x >= y) {
    SDL_RenderDrawPoint(_renderer, cx + x, cy + y);
    SDL_RenderDrawPoint(_renderer, cx + y, cy + x);
    SDL_RenderDrawPoint(_renderer, cx - y, cy + x);
    SDL_RenderDrawPoint(_renderer, cx - x, cy + y);
    SDL_RenderDrawPoint(_renderer, cx - x, cy - y);
    SDL_RenderDrawPoint(_renderer, cx - y, cy - x);
    SDL_RenderDrawPoint(_renderer, cx + y, cy - x);
    SDL_RenderDrawPoint(_renderer, cx + x, cy - y);

    if (err <= 0) {
      y += 1;
      err += 2 * y + 1;
    } else {
      x -= 1;
      err -= 2 * x + 1;
    }
  }
}

void Graphics::drawFilledCircle(int cx, int cy, int radius, SDL_Color color) {

  SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
  for (int dy = -radius; dy <= radius; dy++) {
    int dx = (int)std::sqrt(radius * radius - dy * dy);
    SDL_RenderDrawLine(_renderer, cx - dx, cy + dy, cx + dx, cy + dy);
  }
}

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
  setRenderColor(100, 100, 120, 100);

  _screenWidth = config.width;
  _screenHeight = config.height;
  return true;
}

void Graphics::clear() { SDL_RenderClear(_renderer); }
void Graphics::present() { SDL_RenderPresent(_renderer); }

void Graphics::render() {
  clear();
  present();
}

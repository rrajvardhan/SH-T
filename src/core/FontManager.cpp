#include "FontManager.hpp"
#include "Log.hpp"
#include <SDL2/SDL_ttf.h>

FontManager::FontManager(SDL_Renderer *renderer) : _renderer(renderer) {
  if (TTF_Init() == -1) {
    LOG_ERROR("[FontManager] Failed to initialize SDL_ttf: ", TTF_GetError());
  }

  if (!_renderer) {
    LOG_ERROR("[FontManager] Invalid renderer.");
    return;
  }
  _initialized = true;
  LOG_SUCCESS("[FontManager] SDL_ttf initialized.");
}

FontManager::~FontManager() {
  for (auto &pair : _fonts) {
    if (pair.second) {
      TTF_CloseFont(pair.second);
    }
  }
  _fonts.clear();
  if (_initialized) {
    TTF_Quit();
    LOG_INFO("[AudioManager] SDL_mixer shut down.");
  }
}

TTF_Font *FontManager::loadFont(const std::string &path, int size) {
  TTF_Font *font = TTF_OpenFont(path.c_str(), size);
  if (!font) {
    LOG_ERROR("[FontManager] Failed to load font: ", path, " | ", TTF_GetError());
    return nullptr;
  }

  return font;
}

void FontManager::addFont(const std::string &id, const std::string &path, int size) {
  char *basePath = SDL_GetBasePath();
  if (!basePath) {
    LOG_ERROR("[FontManager] Failed to get base path: ", SDL_GetError());
    return;
  }

  std::string fullpath = std::string(basePath) + path;
  SDL_free(basePath);

  TTF_Font *font = loadFont(fullpath, size);
  auto [it, inserted] = _fonts.insert_or_assign(id, font);
  if (!inserted) {
    LOG_INFO("[FontManager] Replaced existing font ID: ", id);
  }

  LOG_SUCCESS("[FontManager] Font loaded with ID: ", id);
}

TTF_Font *FontManager::getFont(const std::string &id) {
  auto it = _fonts.find(id);
  if (it == _fonts.end()) {
    LOG_ERROR("[FontManager] Font not found for ID: ", id);
    return nullptr;
  }
  return it->second;
}

void FontManager::drawText(const std::string &fontId, const std::string &text, SDL_Color color, SDL_Rect destRect) {
  TTF_Font *font = getFont(fontId);
  if (!font)
    return;

  SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
  if (!surface) {
    LOG_ERROR("[FontManager] Failed to render text: ", TTF_GetError());
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(_renderer, surface);
  SDL_FreeSurface(surface);

  if (!texture) {
    LOG_ERROR("[FontManager] Failed to create text texture: ", SDL_GetError());
    return;
  }

  SDL_RenderCopy(_renderer, texture, nullptr, &destRect);
  SDL_DestroyTexture(texture);
}

void FontManager::unloadFont(const std::string &id) {
  auto it = _fonts.find(id);
  if (it != _fonts.end()) {
    if (it->second)
      TTF_CloseFont(it->second);
    _fonts.erase(it);
    LOG_SUCCESS("[FontManager] Unloaded font with ID: ", id);
  } else {
    LOG_INFO("[FontManager] Tried to unload non-existent font ID: ", id);
  }
}

bool FontManager::hasFont(const std::string &id) const { return _fonts.find(id) != _fonts.end(); }

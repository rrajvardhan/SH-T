#include "Log.hpp"
#include "TextureManager.hpp"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_filesystem.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <string>

TextureManager::TextureManager(SDL_Renderer *renderer) : _renderer(renderer) {
  int flags = IMG_INIT_PNG;
  if (!((IMG_Init(flags)) & flags)) {
    LOG_ERROR("[TextureManager] IMG initialization failed: ", IMG_GetError());
  }

  if (!_renderer) {
    LOG_ERROR("[TextureManager] Invalid renderer.");
  }

  _initialized = true;
  LOG_SUCCESS("[TextureManager] SDL_image initialized.");
}
TextureManager::~TextureManager() {
  for (auto tex : _textures) {
    if (tex.second != nullptr)
      SDL_DestroyTexture(tex.second);
  }
  _textures.clear();
  if (_initialized) {
    IMG_Quit();
    LOG_INFO("[AudioManager] SDL_mixer shut down.");
  }
}

SDL_Texture *TextureManager::loadTexture(const std::string &path) {
  SDL_Texture *texture = nullptr;

  SDL_Surface *surface = IMG_Load(path.c_str());
  if (!surface) {
    LOG_ERROR("[TextureManager] IMG load error :", path, " | ", IMG_GetError());
    return texture;
  }

  texture = SDL_CreateTextureFromSurface(_renderer, surface);
  if (!texture) {
    LOG_ERROR("[TextureManager] Texture creation failed: ", path, " | ", SDL_GetError());
    return texture;
  }

  SDL_FreeSurface(surface);

  return texture;
}

SDL_Texture *TextureManager::getTexture(const std::string &id) {
  auto it = _textures.find(id);
  if (it == _textures.end()) {
    LOG_ERROR("[TextureManager] Texture does not exist, id: ", id);
    return nullptr;
  }
  return it->second;
}

void TextureManager::addTexture(const std::string &id, const std::string &path) {

  char *basePath = SDL_GetBasePath();
  if (!basePath) {
    LOG_ERROR("Failed to get base path: ", SDL_GetError());
    return;
  }

  std::string fullpath = std::string(basePath) + path;
  SDL_free(basePath);

  SDL_Texture *tex = loadTexture(fullpath);
  if (tex == nullptr) {

    LOG_ERROR("[TextureManager] Failed to load texture: ", fullpath);
    return;
  }

  auto [it, inserted] = _textures.insert_or_assign(id, tex);
  if (!inserted) {
    LOG_INFO("[TextureManager] Replaced existing texture ID: ", id);
  }

  LOG_SUCCESS("[TextureManager] Texture added with ID: ", id);
}

void TextureManager::drawTexture(SDL_Texture *texture) {
  if (texture == nullptr) {
    LOG_ERROR("[TextureManager] Tried to draw null texture.");
    return;
  }
  SDL_RenderCopy(_renderer, texture, nullptr, nullptr);
}

void TextureManager::drawTexture(SDL_Texture *texture, SDL_Rect *src, SDL_Rect *dest) {
  if (texture == nullptr) {
    LOG_ERROR("[TextureManager] Tried to draw null texture.");
    return;
  }

  SDL_RenderCopy(_renderer, texture, src, dest);
}

bool TextureManager::hasTexture(const std::string &id) const { return _textures.find(id) != _textures.end(); }

void TextureManager::unloadTexture(const std::string &id) {
  auto it = _textures.find(id);
  if (it != _textures.end()) {
    if (it->second)
      SDL_DestroyTexture(it->second);
    _textures.erase(it);
    LOG_SUCCESS("[TextureManager] Removed texture with ID: ", id);
  } else {
    LOG_INFO("[TextureManager] Tried to remove non-existent texture ID: ", id);
  }
}

#pragma once

#include <SDL2/SDL_render.h>
#include <string>
#include <unordered_map>

class TextureManager
{
public:
  TextureManager(SDL_Renderer* _renderer);
  ~TextureManager();

  SDL_Texture* loadTexture(const std::string& path);
  SDL_Texture* getTexture(const std::string& id);

  void addTexture(const std::string& id, const std::string& path);
  void drawTexture(SDL_Texture* texture);
  void drawTexture(SDL_Texture* texture, SDL_Rect* src, SDL_Rect* dest);
  bool hasTexture(const std::string& id) const;
  void unloadTexture(const std::string& id);

private:
  SDL_Renderer*                                 _renderer;
  std::unordered_map<std::string, SDL_Texture*> _textures;

  bool _initialized = false;
};

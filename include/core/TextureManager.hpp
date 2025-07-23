#pragma once

#include "Log.hpp"
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

  bool addTexture(const std::string& id, const std::string& path);
  void drawTexture(SDL_Texture* texture);
  void drawTexture(SDL_Texture* texture, SDL_Rect* src, SDL_Rect* dest);
  void drawTexture(SDL_Texture*     texture,
                   SDL_Rect*        src,
                   SDL_Rect*        dest,
                   SDL_RendererFlip flip = SDL_FLIP_NONE);
  void drawTexture(SDL_Texture*     texture,
                   SDL_Rect*        src,
                   SDL_Rect*        dest,
                   float            rotation,
                   SDL_Point*       center = nullptr,
                   SDL_RendererFlip flip   = SDL_FLIP_NONE);
  void drawTexture(SDL_Texture* texture, SDL_Rect* src, SDL_Rect* dest, const std::string& flipStr)
  {
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    if (flipStr == "none")
      flip = SDL_FLIP_NONE;
    else if (flipStr == "horizontal")
      flip = SDL_FLIP_HORIZONTAL;
    else if (flipStr == "vertical")
      flip = SDL_FLIP_VERTICAL;
    else if (flipStr == "both")
      flip = (SDL_RendererFlip) (SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
    else
    {
      LOG_ERROR("[TextureManager] Invalid flip.");
      flip = SDL_FLIP_NONE;
    }

    drawTexture(texture, src, dest, flip);
  }
  void drawTexture(SDL_Texture*       texture,
                   SDL_Rect*          src,
                   SDL_Rect*          dest,
                   float              rotation,
                   SDL_Point*         center,
                   const std::string& flipStr)
  {
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    if (flipStr == "none")
      flip = SDL_FLIP_NONE;
    else if (flipStr == "horizontal")
      flip = SDL_FLIP_HORIZONTAL;
    else if (flipStr == "vertical")
      flip = SDL_FLIP_VERTICAL;
    else if (flipStr == "both")
      flip = (SDL_RendererFlip) (SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
    else
    {
      LOG_ERROR("[TextureManager] Invalid flip.");
      flip = SDL_FLIP_NONE;
    }

    drawTexture(texture, src, dest, rotation, center, flip);
  }
  bool hasTexture(const std::string& id) const;
  void unloadTexture(const std::string& id);

  bool renameTexture(const std::string& oldID, const std::string& newID);

  const std::unordered_map<std::string, SDL_Texture*>& getAllTextures() { return _textures; }

private:
  SDL_Renderer*                                 _renderer;
  std::unordered_map<std::string, SDL_Texture*> _textures;

  bool _initialized = false;
};

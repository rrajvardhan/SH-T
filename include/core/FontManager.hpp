#pragma once

#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>

class FontManager {
public:
  FontManager(SDL_Renderer *renderer);
  ~FontManager();

  TTF_Font *getFont(const std::string &id);
  TTF_Font *loadFont(const std::string &path, int size);

  void addFont(const std::string &id, const std::string &path, int size);
  void drawText(const std::string &fontId, const std::string &text, SDL_Color color, SDL_Rect destRect);
  bool hasFont(const std::string &id) const;
  void unloadFont(const std::string &id);

private:
  SDL_Renderer *_renderer;
  std::unordered_map<std::string, TTF_Font *> _fonts;

  bool _initialized = false;
};

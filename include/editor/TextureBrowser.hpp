#pragma once

#include <SDL2/SDL_render.h>
#include <string>
#include <vector>

class TextureManager;

class TextureBrowser
{
public:
  TextureBrowser(TextureManager* textureManager, const std::string& assetDir);

  void draw();

private:
  void loadTextures();

  TextureManager*          _textureManager;
  std::string              _assetDir;
  std::vector<std::string> _textureIDs;
  bool                     _loaded = false;

  float _thumbSize     = 64.f;
  int   _selectedIndex = -1;

  int  _renameIndex = -1;
  char _renameBuffer[128]{};
  int  _contextIndex = -1;
};

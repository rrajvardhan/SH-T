#pragma once

#pragma once
#include "Vector2D.hpp"
#include <SDL2/SDL.h>
#include <string>

struct TextComponent
{
  std::string text    = "Hello, world!";
  std::string fontId  = "default";
  SDL_Color   color   = { 255, 255, 255, 255 };
  Vector2D    size    = { 0.0f, 0.0f };
  Vector2D    offset  = { 0.0f, 0.0f };
  int         layer   = 0;
  bool        visible = true;
};

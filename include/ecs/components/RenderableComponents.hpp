#pragma once

#include "Vector2D.hpp"
#include <SDL2/SDL_pixels.h>

struct Renderable
{
  Vector2D  size;
  SDL_Color color;
  enum ShapeType
  {
    RECT,
    CIRCLE
  } shape
      = RECT;
};

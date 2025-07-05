#pragma once

#include "Vector2D.hpp"

struct Collider
{
  Vector2D size;
  Vector2D offset   = { 0.0f, 0.0f };
  bool     isStatic = false;
};

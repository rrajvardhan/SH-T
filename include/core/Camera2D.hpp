#pragma once

#include "Vector2D.hpp"

class Camera2D
{
public:
  Camera2D(int width = 800, int height = 600)
      : _viewportWidth(width), _viewportHeight(height), _position(0, 0)
  {
  }

  const Vector2D& getPosition() const { return _position; }
  Vector2D        getOffset() const { return _position; }
  int             getViewportWidth() const { return _viewportWidth; }
  int             getViewportHeight() const { return _viewportHeight; }

  void setPosition(const Vector2D& pos) { _position = pos; }
  void setViewport(int width, int height)
  {
    _viewportWidth  = width;
    _viewportHeight = height;
  }

private:
  int      _viewportWidth;
  int      _viewportHeight;
  Vector2D _position;
};

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

  float getZoom() const { return _zoom; }
  void  setZoom(float zoom) { _zoom = std::max(0.1f, zoom); }

private:
  int      _viewportWidth;
  int      _viewportHeight;
  Vector2D _position;

  float _zoom = 1.0f;
};

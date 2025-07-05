#pragma once

#include "ServiceContext.hpp"
#include "World.hpp"

class Editor
{
public:
  Editor(World* world, ServiceContext& ctx);

  void render(); // Renders the full editor UI

  void togglePause() { _paused = !_paused; }
  bool isPaused() const { return _paused; }

private:
  void renderDockspace();
  void renderGamePanel();
  void renderSpawnerPanel();
  void renderDebugInfo();

  SDL_Texture* _gameTexture = nullptr;
  int          _texW        = 0;
  int          _texH        = 0;
  bool         _paused      = false;

  World*         _world;
  ServiceContext _ctx;
};

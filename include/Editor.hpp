#pragma once

#include "ServiceContext.hpp"
#include "Types.hpp"
#include "World.hpp"

class Editor
{
public:
  Editor(World* world, ServiceContext& ctx);

  void render();
  void update();

  bool isActive() { return _active; }

private:
  void renderGamePanel();
  void renderDockspace();
  void renderEntityList();
  void renderComponentInspector();
  void renderDebugInfo();
  void renderControls();
  void spriteThings();

  bool _active = true;

  SDL_Texture* _gameTexture = nullptr;
  int          _texW        = 0;
  int          _texH        = 0;
  bool         _paused      = false;

  World*         _world;
  ServiceContext _ctx;

  Entity _selected = INVALID_ENTITY;
};

#pragma once

#include "ServiceContext.hpp"
#include "Types.hpp"
#include "World.hpp"

class Editor
{
public:
  Editor(World& world, ServiceContext& ctx);

  void render();
  void update();

  void events(SDL_Event* event);

  bool isActive() { return _active; }

private:
  bool _active = true;
  bool _paused = false;

  World&          _world;
  ServiceContext& _ctx;

  Entity _selected = INVALID_ENTITY;
};

#pragma once

#include "AudioBrowser.hpp"
#include "ScriptBrowser.hpp"
#include "ServiceContext.hpp"
#include "TextureBrowser.hpp"
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

  void renderDockspace();
  void renderGamePanel();
  void renderComponentInspector();
  void renderControls();
  void renderEntityPanel();
  void drawSpriteAnimatorUI(Entity& e);

private:
  bool _active = true;
  bool _paused = false;

  World&          _world;
  ServiceContext& _ctx;

  Entity _selected = INVALID_ENTITY;

  std::unique_ptr<TextureBrowser> _btexture;
  std::unique_ptr<ScriptBrowser>  _bscript;
  std::unique_ptr<AudioBrowser>   _baudio;
};

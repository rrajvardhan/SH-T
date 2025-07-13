#include "Editor.hpp"
#include "Panels.hpp"
#include "Types.hpp"
#include "imgui.h"

Editor::Editor(World& world, ServiceContext& ctx) : _world(world), _ctx(ctx)
{
}

void
Editor::update()
{
  ImGuiIO& io = ImGui::GetIO();

  static bool   dragging = false;
  static ImVec2 lastMouse;

  if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
  {
    if (!dragging)
    {
      dragging  = true;
      lastMouse = io.MousePos;
    }
    else
    {
      ImVec2 delta = ImVec2(io.MousePos.x - lastMouse.x, io.MousePos.y - lastMouse.y);
      auto   pos   = _world.getProvider().camera.getPosition();
      _world.getProvider().camera.setPosition(pos - Vector2D(delta.x, delta.y));
      lastMouse = io.MousePos;
    }
  }
  else
  {
    dragging = false;
  }
}

void
Editor::render()
{

  Panels::renderDockspace(_world);

  Panels::renderGamePanel(_active, _ctx, _world);
  Panels::renderDebugInfo();
  Panels::renderEntityList(_world, _selected, _entitiesToDestroy);
  Panels::renderComponentInspector(_selected, _world);
  Panels::renderControls(_active);

  flushDestroyedEntities();
}

void
Editor::flushDestroyedEntities()
{
  auto& ecs = _world.getECS();
  for (Entity e : _entitiesToDestroy)
  {
    ecs.destroyEntity(e);
  }

  _entitiesToDestroy.clear();
}

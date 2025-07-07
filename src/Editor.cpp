#include "CollisionComponents.hpp"
#include "Editor.hpp"
#include "PhysicsComponents.hpp"
#include "imgui.h"

Editor::Editor(World* world, ServiceContext& ctx) : _world(world), _ctx(ctx)
{
}

void
Editor::update()
{
  ImGuiIO& io = ImGui::GetIO();

  static bool   dragging = false;
  static ImVec2 lastMouse;

  if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) // Middle click drag
  {

    if (!dragging)
    {
      dragging  = true;
      lastMouse = io.MousePos;
    }
    else
    {
      ImVec2 delta = ImVec2(io.MousePos.x - lastMouse.x, io.MousePos.y - lastMouse.y);
      _world->getProvider().cameraOffset -= Vector2D(delta.x, delta.y);
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
  renderDebugInfo();
}

void
Editor::renderDebugInfo()
{
  ImGui::Begin("Welcome, to SH!T Services!");
  ImGui::Text("If you're seeing this, ImGui docking + game panel is working.");
  ImGui::End();
}

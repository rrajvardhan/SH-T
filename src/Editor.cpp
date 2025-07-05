#include "CollisionComponents.hpp"
#include "Editor.hpp"
#include "PhysicsComponents.hpp"
#include "imgui.h"

Editor::Editor(World* world, ServiceContext& ctx) : _world(world), _ctx(ctx)
{
}

void
Editor::render()
{
  renderDockspace();
  renderGamePanel();
  renderSpawnerPanel();
  renderDebugInfo();
}

void
Editor::renderDockspace()
{
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  ImGuiWindowFlags          window_flags    = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                  | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::Begin("SH!T DockSpace", nullptr, window_flags);
  ImGui::PopStyleVar(2);

  ImGuiID dockspace_id = ImGui::GetID("SHITDockspace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  ImGui::End();
}

void
Editor::renderGamePanel()
{
  ImGui::Begin("Game");

  ImVec2              gamePanelSize = ImGui::GetContentRegionAvail();
  static SDL_Texture* gameTexture   = nullptr;
  static int          texW = 0, texH = 0;
  int                 newW = static_cast<int>(gamePanelSize.x);
  int                 newH = static_cast<int>(gamePanelSize.y);

  if (newW != texW || newH != texH || gameTexture == nullptr)
  {
    if (gameTexture)
      SDL_DestroyTexture(gameTexture);

    texW        = newW;
    texH        = newH;
    gameTexture = SDL_CreateTexture(_ctx.graphics->getRenderer(),
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    texW,
                                    texH);
  }

  SDL_SetRenderTarget(_ctx.graphics->getRenderer(), gameTexture);
  SDL_SetRenderDrawColor(_ctx.graphics->getRenderer(), 20, 20, 20, 255);
  SDL_RenderClear(_ctx.graphics->getRenderer());
  _world->setViewport(newW, newH);
  _world->render();
  SDL_SetRenderTarget(_ctx.graphics->getRenderer(), nullptr);

  if (gameTexture)
    ImGui::Image((ImTextureID) gameTexture, gamePanelSize);

  ImGui::End();
}

void
Editor::renderSpawnerPanel()
{
  ImGui::Begin("Spawner");

  static float pos[2]  = { 500.f, 0.f };
  static float size[2] = { 100.f, 100.f };

  ImGui::Checkbox("Pause World", &_paused);

  ImGui::InputFloat2("Position", pos, "%.1f");
  ImGui::InputFloat2("Size", size, "%.1f");

  if (ImGui::Button("Spawn Collider Entity"))
  {
    auto&  ecs = _world->getECS();
    Entity e   = ecs.createEntity();
    ecs.addComponent<Transform>(e, { Vector2D{ pos[0], pos[1] } });

    Collider c;
    c.size   = { size[0], size[1] };
    c.offset = { 0.f, 0.f };

    ecs.addComponent<Collider>(e, c);
  }

  ImGui::End();
}

void
Editor::renderDebugInfo()
{
  ImGui::Begin("Welcome, to SH!T Services!");
  ImGui::Text("If you're seeing this, ImGui docking + game panel is working.");
  ImGui::End();
}

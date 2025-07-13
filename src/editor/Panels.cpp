#include "CameraComponents.hpp"
#include "CollisionComponents.hpp"
#include "PhysicsComponents.hpp"
#include "RenderableComponents.hpp"
#include "ServiceContext.hpp"
#include "SpriteComponents.hpp"
#include "Types.hpp"
#include "UtilComponents.hpp"
#include "World.hpp"
#include "imgui.h"
#include <SDL2/SDL_render.h>
#include <string>
#include <vector>

namespace Panels
{

void
renderDockspace(World& world)
{

  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  ImGuiWindowFlags          window_flags    = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  ImGuiViewport*            viewport        = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);

  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                  | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  ImGui::Begin("SH!T DockSpace", nullptr, window_flags);
  ImGuiID dockspace_id = ImGui::GetID("SHITDockspace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("Systems"))
    {
      int index = 0;
      for (auto& sys : world.getSystems())
      {
        ImGui::PushID(index++);

        ImGui::TextUnformatted(sys.name.c_str());
        ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 50);
        ImGui::Checkbox("##enabled", &sys.enabled);
        ImGui::PopID();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  ImGui::End();
}

void
renderGamePanel(bool& active, ServiceContext& ctx, World& world)
{

  std::string label = active ? "Game (Edit Mode)" : "Game";

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin(label.c_str());

  ImVec2              gamePanelSize = ImGui::GetContentRegionAvail();
  static SDL_Texture* gameTexture   = nullptr;

  static int texW = 0, texH = 0;
  int        newW = static_cast<int>(gamePanelSize.x);
  int        newH = static_cast<int>(gamePanelSize.y);
  if (newW != texW || newH != texH || gameTexture == nullptr)
  {
    if (gameTexture)
      SDL_DestroyTexture(gameTexture);
    texW        = newW;
    texH        = newH;
    gameTexture = SDL_CreateTexture(ctx.graphics->getRenderer(),
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    texW,
                                    texH);
  }

  SDL_SetRenderTarget(ctx.graphics->getRenderer(), gameTexture);
  ctx.graphics->clear();

  world.getProvider().camera.setViewport(newW, newH);
  world.render();

  SDL_SetRenderTarget(ctx.graphics->getRenderer(), nullptr);
  if (gameTexture)
    ImGui::Image((ImTextureID) gameTexture, gamePanelSize);

  ImGui::PopStyleVar(1);
  ImGui::End();
}

void
renderEntityList(World& world, Entity& selected, std::vector<Entity>& destroyQueue)
{
  ImGui::Begin("Entities");
  auto& ecs = world.getECS();

  if (ImGui::Button("Add Entity"))
  {
    Entity newEntity = ecs.createEntity();
    ecs.addComponent<Identification>(newEntity,
                                     { "Entity" + std::to_string((uint32_t) newEntity) });
    ecs.addComponent<Transform>(newEntity, {});

    selected = newEntity;
  }

  for (Entity entity : ecs.getEntities())
  {
    std::string label = "Entity " + std::to_string((uint32_t) entity);

    if (ecs.hasComponent<Identification>(entity))
    {
      const auto& id = ecs.getComponent<Identification>(entity);
      label          = id.name + "  [id: " + std::to_string((uint32_t) entity) + " ]";
    }

    if (ImGui::Selectable(label.c_str(), selected == entity))
    {
      selected = entity;
    }

    if (ImGui::BeginPopupContextItem(std::to_string(entity).c_str()))
    {
      if (ImGui::MenuItem("Delete"))
      {
        destroyQueue.push_back(selected);
      }
      ImGui::EndPopup();
    }
  }
  ImGui::End();
}

void
renderComponentInspector(Entity& selected, World& world)
{
  if (selected == INVALID_ENTITY)
    return;

  ImGui::Begin("Inspector");

  auto& ecs = world.getECS();

  if (ecs.hasComponent<Identification>(selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Identification"))
    {
      auto& i = ecs.getComponent<Identification>(selected);
      char  nameBuffer[128];
      strncpy(nameBuffer, i.name.c_str(), sizeof(nameBuffer));
      nameBuffer[sizeof(nameBuffer) - 1] = '\0';

      if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
      {
        i.name = std::string(nameBuffer);
      }

      static char groupBuffer[128] = "";
      ImGui::InputText("Group", groupBuffer, sizeof(groupBuffer));
    }
  }

  if (ecs.hasComponent<Transform>(selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Transform"))
    {
      auto& t = ecs.getComponent<Transform>(selected);
      ImGui::DragFloat2("Position", &t.position.x, 1.0f);
    }
  }

  if (ecs.hasComponent<RigidBody>(selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("RigidBody"))
    {
      auto& rb = ecs.getComponent<RigidBody>(selected);
      ImGui::DragFloat("Mass", &rb.mass, 1.0f);
      ImGui::DragFloat2("Velocity", &rb.velocity.x, 1.0f);
    }
  }

  if (ecs.hasComponent<Force>(selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Force"))
    {
      auto& f = ecs.getComponent<Force>(selected);
      ImGui::Text("Force");
      ImGui::DragFloat2("Force", &f.vector.x, 1.0f);
    }
  }

  if (ecs.hasComponent<Collider>(selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Collider"))
    {
      auto& c = ecs.getComponent<Collider>(selected);
      ImGui::Checkbox("static", &c.isStatic);
      ImGui::DragFloat2("Collider Size", &c.size.x, 1.0f);
      ImGui::DragFloat2("Collider Offset", &c.offset.x, 1.0f);
    }
  }

  if (ecs.hasComponent<Renderable>(selected))
  {

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Renderable"))
    {
      auto& r = ecs.getComponent<Renderable>(selected);
      ImGui::DragFloat2("Renderable Size", &r.size.x, 1.0f);
      float color[4]
          = { r.color.r / 255.0f, r.color.g / 255.0f, r.color.b / 255.0f, r.color.a / 255.0f };
      if (ImGui::ColorEdit4("Color", color))
      {
        r.color.r = static_cast<Uint8>(color[0] * 255.0f);
        r.color.g = static_cast<Uint8>(color[1] * 255.0f);
        r.color.b = static_cast<Uint8>(color[2] * 255.0f);
        r.color.a = static_cast<Uint8>(color[3] * 255.0f);
      }
    }
  }

  if (ecs.hasComponent<FollowCamera>(selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("FollowCamera"))
    {
      auto& cam = ecs.getComponent<FollowCamera>(selected);
      ImGui::Checkbox("Active ", &cam.isActive);
      int tmp = static_cast<int>(cam.target);
      if (ImGui::InputInt("Target", &tmp))
      {
        cam.target = static_cast<Entity>(tmp);
      }
    }
  }

  ImGui::Separator();
  if (ImGui::Button("Add Component"))
  {
    ImGui::OpenPopup("AddComponentPopup");
  }

  if (ImGui::BeginPopup("AddComponentPopup"))
  {
    if (!ecs.hasComponent<Identification>(selected) && ImGui::MenuItem("Identification"))
      ecs.addComponent<Identification>(selected, {});

    if (!ecs.hasComponent<RigidBody>(selected) && ImGui::MenuItem("RigidBody"))
      ecs.addComponent<RigidBody>(selected, {});

    if (!ecs.hasComponent<Collider>(selected) && ImGui::MenuItem("Collider"))
      ecs.addComponent<Collider>(selected, {});

    if (!ecs.hasComponent<Force>(selected) && ImGui::MenuItem("Force"))
      ecs.addComponent<Force>(selected, {});

    if (!ecs.hasComponent<Renderable>(selected) && ImGui::MenuItem("Renderable"))
      ecs.addComponent<Renderable>(selected, {});

    if (!ecs.hasComponent<Sprite>(selected) && ImGui::MenuItem("Sprite"))
      ecs.addComponent<Sprite>(selected, {});

    if (!ecs.hasComponent<FollowCamera>(selected) && ImGui::MenuItem("FollowCamera"))
      ecs.addComponent<FollowCamera>(selected, {});

    if (!ecs.hasComponent<SpriteAnimator>(selected) && ImGui::MenuItem("SpriteAnimator"))
      ecs.addComponent<SpriteAnimator>(selected, {});

    ImGui::EndPopup();
  }

  ImGui::End();
}

void
renderControls(bool& active)
{
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));

  ImGui::Begin("Toolbar",
               nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
                   | ImGuiWindowFlags_AlwaysAutoResize);

  if (ImGui::Button(active ? "Play " : "Pause"))
  {
    active = !active;
  }

  ImGui::SameLine();

  if (ImGui::Button("Reset"))
  {
  }

  ImGui::End();
  ImGui::PopStyleVar(2);
}

void
renderDebugInfo()
{
  ImGui::Begin("Welcome, to SH!T Services!");
  ImGui::Text("If you're seeing this, ImGui docking + game panel is working.");
  ImGui::End();
}

}

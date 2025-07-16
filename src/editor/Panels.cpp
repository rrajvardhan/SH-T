#include "CameraComponents.hpp"
#include "CollisionComponents.hpp"
#include "Log.hpp"
#include "Panels.hpp"
#include "PhysicsComponents.hpp"
#include "RenderableComponents.hpp"
#include "ScriptBrowser.hpp"
#include "ServiceContext.hpp"
#include "SpriteComponents.hpp"
#include "TextureBrowser.hpp"
#include "Types.hpp"
#include "UtilComponents.hpp"
#include "World.hpp"
#include "imgui.h"
#include <SDL2/SDL_render.h>
#include <fstream>
#include <string>
#include <vector>

std::string
loadFileToString(const std::string& path)
{
  std::ifstream     in(path);
  std::stringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

void
saveStringToFile(const std::string& path, const std::string& content)
{
  std::ofstream out(path);
  out << content;
}

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
    if (ImGui::BeginMenu("Debug"))
    {

      ImGui::Checkbox("DebugDraw System##enabled", &world.isDebug);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  ImGui::End();
}

void
renderGamePanel(bool& active, ServiceContext& ctx, World& world)
{

  static float currentZoom = 1.0f;
  static float targetZoom  = 1.0f;

  static bool   dragging = false;
  static ImVec2 lastMouse;

  std::string label = active ? "Game (Edit Mode)" : "Game";
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin(label.c_str());

  if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)
      && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
  {
    active = true;
  }

  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && active)
  {
    float scroll = ImGui::GetIO().MouseWheel;
    if (scroll != 0.0f)
    {
      targetZoom += scroll * 0.1f;
      targetZoom = std::clamp(targetZoom, 0.25f, 4.0f);
    }

    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle))
    {
      if (!dragging)
      {
        dragging  = true;
        lastMouse = ImGui::GetIO().MousePos;
      }
      else
      {
        ImVec2 delta = ImVec2(ImGui::GetIO().MousePos.x - lastMouse.x,
                              ImGui::GetIO().MousePos.y - lastMouse.y);

        auto pos = world.getCamera().getPosition();
        world.getCamera().setPosition(pos - Vector2D(delta.x, delta.y));
        lastMouse = ImGui::GetIO().MousePos;
      }
    }
    else
    {
      dragging = false;
    }
  }

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

  world.getCamera().setViewport(newW, newH);
  world.render();

  currentZoom = currentZoom + (targetZoom - currentZoom) * 0.1f;
  world.getCamera().setZoom(currentZoom);

  SDL_SetRenderTarget(ctx.graphics->getRenderer(), nullptr);
  if (gameTexture)
    ImGui::Image((ImTextureID) gameTexture, gamePanelSize);

  ImGui::PopStyleVar(1);
  ImGui::End();
}

void
renderEntityPanel(World& world)
{

  static std::vector<Entity> destroyQueue;

  auto& ecs = world.getECS();
  ImGui::Begin("Entities");

  // === Entity Creation ===
  if (ImGui::Button("+ Add Entity"))
  {
    Entity newEntity = ecs.createEntity();
    ecs.addComponent<Identification>(newEntity,
                                     { "Entity | " + std::to_string((Entity) newEntity) });
  }

  ImGui::SeparatorText("Scene Entities");

  // === Entity List ===
  for (Entity entity : ecs.getEntities())
  {
    std::string label = "Entity | " + std::to_string((uint32_t) entity);
    if (ecs.hasComponent<Identification>(entity))
      label = ecs.getComponent<Identification>(entity).name;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_FramePadding;

    bool open = ImGui::TreeNodeEx(label.c_str(), flags);

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 20);
    if (ImGui::SmallButton(("X##delete" + std::to_string(entity)).c_str()))
      ImGui::OpenPopup(("DeletePopup" + std::to_string(entity)).c_str());
    if (ImGui::BeginPopup(("DeletePopup" + std::to_string(entity)).c_str()))
    {
      ImGui::Text("Delete entity '%s'?", label.c_str());

      if (ImGui::Button("Yes"))
      {
        destroyQueue.push_back(entity);
        ImGui::CloseCurrentPopup();
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel"))
        ImGui::CloseCurrentPopup();

      ImGui::EndPopup();
    }

    ImGuiTreeNodeFlags header = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_CollapsingHeader;
    if (open)
    {
      // === Component Editors ===
      if (ecs.hasComponent<Identification>(entity))
      {

        if (ImGui::CollapsingHeader("Identification", header))
        {
          auto& i = ecs.getComponent<Identification>(entity);
          char  nameBuffer[128];
          strncpy(nameBuffer, i.name.c_str(), sizeof(nameBuffer));
          nameBuffer[sizeof(nameBuffer) - 1] = '\0';

          if (ImGui::InputText(
                  "Name", nameBuffer, sizeof(nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
          {
            if (ImGui::IsItemDeactivatedAfterEdit())
              i.name = std::string(nameBuffer);
          }

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Identification" + std::to_string(entity)).c_str()))
          {
            ecs.removeComponent<Identification>(entity);
          }

          ImGui::Separator();
        }
      }

      if (ecs.hasComponent<Transform>(entity))
      {
        if (ImGui::CollapsingHeader("Transform", header))
        {
          auto& t = ecs.getComponent<Transform>(entity);
          ImGui::DragFloat2("Position", &t.position.x, 1.0f);
          ImGui::DragFloat("Rotation", &t.rotation, 1.0f);

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Transform" + std::to_string(entity)).c_str()))
          {
            ecs.removeComponent<Transform>(entity);
          }
          ImGui::Separator();
        }
      }

      if (ecs.hasComponent<RigidBody>(entity))
      {
        if (ImGui::CollapsingHeader("RigidBody", header))
        {
          auto& rb = ecs.getComponent<RigidBody>(entity);
          ImGui::DragFloat("Mass", &rb.mass, 1.0f);
          ImGui::DragFloat2("Velocity", &rb.velocity.x, 1.0f);

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##RigidBody" + std::to_string(entity)).c_str()))
          {
            ecs.removeComponent<RigidBody>(entity);
          }
          ImGui::Separator();
        }
      }

      if (ecs.hasComponent<Force>(entity))
      {
        if (ImGui::CollapsingHeader("Force", header))
        {
          auto& f = ecs.getComponent<Force>(entity);
          ImGui::DragFloat2("Force", &f.vector.x, 1.0f);

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Force" + std::to_string(entity)).c_str()))
          {
            ecs.removeComponent<Force>(entity);
          }
          ImGui::Separator();
        }
      }

      if (ecs.hasComponent<Collider>(entity))
      {
        if (ImGui::CollapsingHeader("Collider", header))
        {
          auto& c = ecs.getComponent<Collider>(entity);
          ImGui::Checkbox("Static", &c.isStatic);
          ImGui::DragFloat2("Size##Collider", &c.size.x, 1.0f);
          ImGui::DragFloat2("Offset##Collider", &c.offset.x, 1.0f);

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Collider" + std::to_string(entity)).c_str()))
          {
            ecs.removeComponent<Collider>(entity);
          }
          ImGui::Separator();
        }
      }

      if (ecs.hasComponent<Renderable>(entity))
      {
        if (ImGui::CollapsingHeader("Renderable", header))
        {
          auto& r = ecs.getComponent<Renderable>(entity);
          ImGui::DragFloat2("Size##Renderable", &r.size.x, 1.0f);
          float color[4]
              = { r.color.r / 255.0f, r.color.g / 255.0f, r.color.b / 255.0f, r.color.a / 255.0f };
          if (ImGui::ColorEdit4("Color", color))
          {
            r.color.r = static_cast<Uint8>(color[0] * 255.0f);
            r.color.g = static_cast<Uint8>(color[1] * 255.0f);
            r.color.b = static_cast<Uint8>(color[2] * 255.0f);
            r.color.a = static_cast<Uint8>(color[3] * 255.0f);
          }

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Renderable" + std::to_string(entity)).c_str()))
          {
            ecs.removeComponent<Renderable>(entity);
          }
          ImGui::Separator();
        }
      }

      if (ecs.hasComponent<FollowCamera>(entity))
      {
        if (ImGui::CollapsingHeader("FollowCamera", header))
        {
          auto& cam = ecs.getComponent<FollowCamera>(entity);
          ImGui::Checkbox("Active", &cam.isActive);
          int tmp = static_cast<int>(cam.target);
          if (ImGui::InputInt("Target", &tmp))
            cam.target = static_cast<Entity>(tmp);

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##FollowCamera" + std::to_string(entity)).c_str()))
          {
            ecs.removeComponent<FollowCamera>(entity);
          }
          ImGui::Separator();
        }
      }

      if (ecs.hasComponent<Sprite>(entity))
      {
        auto& sprite = ecs.getComponent<Sprite>(entity);

        if (ImGui::CollapsingHeader("Sprite"))
        {
          static char   nameBuffer[128] = "";
          static Entity lastEntity      = INVALID_ENTITY;
          if (lastEntity != entity)
          {
            strncpy(nameBuffer, sprite.name.c_str(), sizeof(nameBuffer));
            nameBuffer[sizeof(nameBuffer) - 1] = '\0';
            lastEntity                         = entity;
          }

          if (ImGui::InputText("Name##Sprite",
                               nameBuffer,
                               sizeof(nameBuffer),
                               ImGuiInputTextFlags_EnterReturnsTrue))
          {
            sprite.name = nameBuffer;
          }

          ImGui::DragInt4("Src Rect##Sprite", (int*) &sprite.srcRect);
          ImGui::DragFloat("Scale##Sprite", &sprite.scale, 0.01f, 0.1f);
          ImGui::DragFloat2("Offset##Sprite", &sprite.offset.x);
          ImGui::InputInt("Layer", &sprite.layer);

          const char* flipLabels[] = { "None", "Horizontal", "Vertical" };
          int         flipMode     = (int) sprite.flip;
          if (ImGui::Combo("Flip", &flipMode, flipLabels, IM_ARRAYSIZE(flipLabels)))
            sprite.flip = (SDL_RendererFlip) flipMode;

          const char* blendLabels[] = { "None", "Blend", "Add", "Mod" };
          int         blendIndex    = (int) sprite.blendMode;
          if (ImGui::Combo("Blend Mode", &blendIndex, blendLabels, IM_ARRAYSIZE(blendLabels)))
            sprite.blendMode = (SDL_BlendMode) blendIndex;

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Sprite" + std::to_string(entity)).c_str()))
          {
            ecs.removeComponent<Sprite>(entity);
          }
          ImGui::Separator();
        }
      }

      // === Add Component ===
      if (ImGui::Button("Add Component"))
        ImGui::OpenPopup(("AddComponentPopup" + std::to_string(entity)).c_str());

      if (ImGui::BeginPopup(("AddComponentPopup" + std::to_string(entity)).c_str()))
      {
        if (!ecs.hasComponent<Identification>(entity) && ImGui::MenuItem("Identification"))
          ecs.addComponent<Identification>(entity, {});

        if (!ecs.hasComponent<Transform>(entity) && ImGui::MenuItem("Transform"))
          ecs.addComponent<Transform>(entity, {});

        if (!ecs.hasComponent<RigidBody>(entity) && ImGui::MenuItem("RigidBody"))
          ecs.addComponent<RigidBody>(entity, {});

        if (!ecs.hasComponent<Collider>(entity) && ImGui::MenuItem("Collider"))
          ecs.addComponent<Collider>(entity, {});

        if (!ecs.hasComponent<Force>(entity) && ImGui::MenuItem("Force"))
          ecs.addComponent<Force>(entity, {});

        if (!ecs.hasComponent<Renderable>(entity) && ImGui::MenuItem("Renderable"))
          ecs.addComponent<Renderable>(entity, {});

        if (!ecs.hasComponent<FollowCamera>(entity) && ImGui::MenuItem("FollowCamera"))
          ecs.addComponent<FollowCamera>(entity, {});

        if (!ecs.hasComponent<Sprite>(entity) && ImGui::MenuItem("Sprite"))
          ecs.addComponent<Sprite>(entity, {});

        ImGui::EndPopup();
      }

      ImGui::TreePop();
    }
    ImGui::Separator();
  }

  ImGui::End();

  for (Entity e : destroyQueue)
    ecs.destroyEntity(e);
  destroyQueue.clear();
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
renderResources(ServiceContext& ctx, World& world)
{

  static TextureAssetBrowser textureBrowser(ctx.texture, "assets/textures");
  textureBrowser.draw();

  static ScriptBrowser scriptBrowser("scripts", world.getScriptSystem());
  scriptBrowser.draw();
}
}

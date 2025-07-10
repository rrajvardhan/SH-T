#include "CameraComponents.hpp"
#include "CollisionComponents.hpp"
#include "Editor.hpp"
#include "PhysicsComponents.hpp"
#include "RenderableComponents.hpp"
#include "SpriteComponents.hpp"
#include "Types.hpp"
#include "UtilComponents.hpp"
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

  renderDockspace();
  renderGamePanel();

  renderDebugInfo();
  renderEntityList();
  renderComponentInspector();
  renderControls();

  spriteThings();

  flushDestroyedEntities();
}

void
Editor::renderDockspace()
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

  ImGui::End();
}

void
Editor::renderGamePanel()
{

  std::string label = _active ? "Game (Edit Mode)" : "Game";

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
    gameTexture = SDL_CreateTexture(_ctx.graphics->getRenderer(),
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    texW,
                                    texH);
  }

  SDL_SetRenderTarget(_ctx.graphics->getRenderer(), gameTexture);
  _ctx.graphics->setDrawColor({ 0, 0, 0, 0 });
  _ctx.graphics->clear();

  _world->setViewport(newW, newH);
  _world->render();

  SDL_SetRenderTarget(_ctx.graphics->getRenderer(), nullptr);
  if (gameTexture)
    ImGui::Image((ImTextureID) gameTexture, gamePanelSize);

  ImGui::PopStyleVar(1);
  ImGui::End();
}

void
Editor::renderEntityList()
{
  ImGui::Begin("Entities");
  auto& ecs = _world->getECS();

  if (ImGui::Button("Add Entity"))
  {
    Entity newEntity = ecs.createEntity();
    ecs.addComponent<Identification>(newEntity,
                                     { "Entity" + std::to_string((uint32_t) newEntity) });
    ecs.addComponent<Transform>(newEntity, {});

    _selected = newEntity;
  }

  for (Entity entity : ecs.getEntities())
  {
    std::string label = "Entity " + std::to_string((uint32_t) entity);

    if (ecs.hasComponent<Identification>(entity))
    {
      const auto& id = ecs.getComponent<Identification>(entity);
      label          = id.name + "  [id: " + std::to_string((uint32_t) entity) + " ]";
    }

    if (ImGui::Selectable(label.c_str(), _selected == entity))
    {
      _selected = entity;
    }

    if (ImGui::BeginPopupContextItem(std::to_string(entity).c_str()))
    {
      if (ImGui::MenuItem("Delete"))
      {
        markEntityForDeletion(entity);
      }
      ImGui::EndPopup();
    }
  }
  ImGui::End();

  int index = 0;
  ImGui::Begin("System Control");
  for (auto& sys : _world->getSystems())
  {
    int originalOrder = sys.order;
    ImGui::Separator();

    ImGui::PushID(index++);
    if (ImGui::CollapsingHeader(sys.name.c_str()))
    {
      ImGui::Checkbox("Enabled", &sys.enabled);

      static char phaseBuffer[128];
      strncpy(phaseBuffer, sys.phase.c_str(), sizeof(phaseBuffer));
      phaseBuffer[sizeof(phaseBuffer) - 1] = '\0';
      if (ImGui::InputText("Edit Phase", phaseBuffer, sizeof(phaseBuffer)))
      {
        sys.phase = std::string(phaseBuffer);
      }

      if (ImGui::InputInt("Edit Order", &sys.order))
      {
        if (sys.order != originalOrder)
          _world->resortSystems();
      }
    }
    ImGui::PopID();
  }

  ImGui::End();
}

void
Editor::renderComponentInspector()
{
  if (_selected == INVALID_ENTITY)
    return;

  ImGui::Begin("Inspector");

  auto& ecs = _world->getECS();

  if (ecs.hasComponent<Identification>(_selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Identification"))
    {
      auto& i = ecs.getComponent<Identification>(_selected);
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

  if (ecs.hasComponent<Transform>(_selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Transform"))
    {
      auto& t = ecs.getComponent<Transform>(_selected);
      ImGui::DragFloat2("Position", &t.position.x, 1.0f);
    }
  }

  if (ecs.hasComponent<RigidBody>(_selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("RigidBody"))
    {
      auto& rb = ecs.getComponent<RigidBody>(_selected);
      ImGui::DragFloat("Mass", &rb.mass, 1.0f);
      ImGui::DragFloat2("Velocity", &rb.velocity.x, 1.0f);
    }
  }

  if (ecs.hasComponent<Force>(_selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Force"))
    {
      auto& f = ecs.getComponent<Force>(_selected);
      ImGui::Text("Force");
      ImGui::DragFloat2("Force", &f.vector.x, 1.0f);
    }
  }

  if (ecs.hasComponent<Collider>(_selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Collider"))
    {
      auto& c = ecs.getComponent<Collider>(_selected);
      ImGui::Checkbox("static", &c.isStatic);
      ImGui::DragFloat2("Collider Size", &c.size.x, 1.0f);
      ImGui::DragFloat2("Collider Offset", &c.offset.x, 1.0f);
    }
  }

  if (ecs.hasComponent<Renderable>(_selected))
  {

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Renderable"))
    {
      auto& r = ecs.getComponent<Renderable>(_selected);
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

  if (ecs.hasComponent<FollowCamera>(_selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("FollowCamera"))
    {
      auto& cam = ecs.getComponent<FollowCamera>(_selected);
      ImGui::Checkbox("Active ", &cam.isActive);
      int tmp = static_cast<int>(cam.target);
      if (ImGui::InputInt("Target", &tmp))
      {
        cam.target = static_cast<Entity>(tmp);
      }
    }
  }

  if (ecs.hasComponent<Sprite>(_selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Sprite"))
    {
      auto& s = ecs.getComponent<Sprite>(_selected);
      ImGui::Text("Sprite");
      ImGui::Text("Rect");
      ImGui::DragInt("Src X", &s.srcRect.x, 1.0f);
      ImGui::DragInt("Src Y", &s.srcRect.y, 1.0f);
      ImGui::DragInt("Src W", &s.srcRect.w, 1.0f);
      ImGui::DragInt("Src H", &s.srcRect.h, 1.0f);
      ImGui::DragFloat("Scale", &s.scale, 0.1f);
      ImGui::DragFloat2("Sprite Offset", &s.offset.x, 1.0f);
      ImGui::Combo("Flip", (int*) &s.flip, "None\0Horizontal\0Vertical\0");
    }
  }

  if (ecs.hasComponent<SpriteAnimator>(_selected))
  {
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Animation"))
    {
      auto& anim = ecs.getComponent<SpriteAnimator>(_selected);
      if (!anim.animations.empty())
      {
        std::vector<const char*> names;
        for (auto& [key, _] : anim.animations)
          names.push_back(key.c_str());

        int currentIndex = 0;
        for (size_t i = 0; i < names.size(); ++i)
        {
          if (anim.currentAnim == names[i])
          {
            currentIndex = i;
            break;
          }
        }

        if (ImGui::Combo("Current Animation", &currentIndex, names.data(), names.size()))
        {
          anim.currentAnim  = names[currentIndex];
          anim.currentFrame = 0;
          anim.timer        = 0.0f;
        }
      }

      ImGui::Text("Current Frame: %d", anim.currentFrame);
      ImGui::Text("Timer: %.2f ms", anim.timer);

      if (ImGui::TreeNode("Animations"))
      {
        for (auto& [name, animation] : anim.animations)
        {
          if (ImGui::TreeNode(name.c_str()))
          {
            ImGui::InputInt("Speed[ms]", &animation.speed);
            for (size_t i = 0; i < animation.frames.size(); ++i)
            {
              auto& frame = animation.frames[i];
              ImGui::Text("Frame %d", static_cast<int>(i));
              ImGui::Text("React");
              ImGui::Text(
                  "(%d, %d, %d, %d)", frame.rect.x, frame.rect.y, frame.rect.w, frame.rect.h);
              ImGui::Text("Offset");
              ImGui::Text("(%f, %f)", frame.offset.x, frame.offset.y);
            }
            ImGui::TreePop();
          }
        }
        ImGui::TreePop();
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
    if (!ecs.hasComponent<Identification>(_selected) && ImGui::MenuItem("Identification"))
      ecs.addComponent<Identification>(_selected, {});

    if (!ecs.hasComponent<RigidBody>(_selected) && ImGui::MenuItem("RigidBody"))
      ecs.addComponent<RigidBody>(_selected, {});

    if (!ecs.hasComponent<Collider>(_selected) && ImGui::MenuItem("Collider"))
      ecs.addComponent<Collider>(_selected, {});

    if (!ecs.hasComponent<Force>(_selected) && ImGui::MenuItem("Force"))
      ecs.addComponent<Force>(_selected, {});

    if (!ecs.hasComponent<Renderable>(_selected) && ImGui::MenuItem("Renderable"))
      ecs.addComponent<Renderable>(_selected, {});

    if (!ecs.hasComponent<Sprite>(_selected) && ImGui::MenuItem("Sprite"))
      ecs.addComponent<Sprite>(_selected, {});

    if (!ecs.hasComponent<FollowCamera>(_selected) && ImGui::MenuItem("FollowCamera"))
      ecs.addComponent<FollowCamera>(_selected, {});

    if (!ecs.hasComponent<SpriteAnimator>(_selected) && ImGui::MenuItem("SpriteAnimator"))
      ecs.addComponent<SpriteAnimator>(_selected, {});

    ImGui::EndPopup();
  }

  ImGui::End();
}

void
Editor::spriteThings()
{
}

void
Editor::renderControls()
{
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));

  ImGui::Begin("Toolbar",
               nullptr,
               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
                   | ImGuiWindowFlags_AlwaysAutoResize);

  if (ImGui::Button(_active ? "Play " : "Pause"))
  {
    _active = !_active;
  }

  ImGui::SameLine();

  if (ImGui::Button("Reset"))
  {
  }

  ImGui::End();
  ImGui::PopStyleVar(2);
}

void
Editor::renderDebugInfo()
{
  ImGui::Begin("Welcome, to SH!T Services!");
  ImGui::Text("If you're seeing this, ImGui docking + game panel is working.");
  ImGui::End();
}

void
Editor::markEntityForDeletion(Entity e)
{
  _entitiesToDestroy.push_back(e);
  if (_selected == e)
    _selected = INVALID_ENTITY;
}

void
Editor::flushDestroyedEntities()
{
  auto& ecs = _world->getECS();
  for (Entity e : _entitiesToDestroy)
  {
    ecs.destroyEntity(e);
  }

  _entitiesToDestroy.clear();
}

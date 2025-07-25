#include "CameraComponents.hpp"
#include "CollisionComponents.hpp"
#include "ComponentSerializer.hpp"
#include "Editor.hpp"
#include "JsonSerializer.hpp"
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

  ImGui::Begin("DockSpace", nullptr, window_flags);
  ImGuiID dockspace_id = ImGui::GetID("Dockspace");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("Debug"))
    {

      ImGui::Checkbox("DebugDraw System##enabled", &_world.isDebug);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  ImGui::End();
}

void
Editor::renderGamePanel()
{

  static float currentZoom = 1.0f;
  static float targetZoom  = 1.0f;

  static bool   dragging = false;
  static ImVec2 lastMouse;

  std::string label = _active ? "Scene (Edit Mode)" : "Scene";
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin(label.c_str());

  if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)
      && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
  {
    _active = true;
  }

  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) && _active)
  {
    float scroll = ImGui::GetIO().MouseWheel;
    if (scroll != 0.0f)
    {
      targetZoom += scroll * 0.1f;
      targetZoom = std::clamp(targetZoom, 0.1f, 10.0f);

      currentZoom = currentZoom + (targetZoom - currentZoom);
      _world.getCamera().setZoom(currentZoom);
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

        auto pos = _world.getCamera().getPosition();
        _world.getCamera().setPosition(pos - Vector2D(delta.x, delta.y));
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
    gameTexture = SDL_CreateTexture(_ctx.graphics->getRenderer(),
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    texW,
                                    texH);
  }

  SDL_SetRenderTarget(_ctx.graphics->getRenderer(), gameTexture);
  _ctx.graphics->clear();

  _world.getCamera().setViewport(newW, newH);
  _world.render();

  SDL_SetRenderTarget(_ctx.graphics->getRenderer(), nullptr);
  if (gameTexture)
    ImGui::Image((ImTextureID) gameTexture, gamePanelSize);

  ImGui::PopStyleVar(1);
  ImGui::End();
}

void
Editor::renderEntityPanel()
{

  static std::vector<Entity> destroyQueue;

  auto& _ecs = _world.getECS();
  ImGui::Begin("Entities");

  // === Entity Creation ===
  if (ImGui::Button("+ Add Entity"))
  {
    Entity newEntity = _ecs.createEntity();
    _ecs.addComponent(newEntity, Transform{});
  }

  ImGui::SeparatorText("Scene Entities");

  // === Entity List ===
  for (Entity entity : _ecs.getEntities())
  {
    std::string label = "Entity | " + std::to_string((uint32_t) entity);
    if (_ecs.hasComponent<Identification>(entity))
      label = _ecs.getComponent<Identification>(entity).name + " | "
              + std::to_string((uint32_t) entity);
    ;

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
      if (_ecs.hasComponent<Identification>(entity))
      {

        if (ImGui::CollapsingHeader("Identification", header))
        {
          auto& i = _ecs.getComponent<Identification>(entity);
          char  textureIdBuffer[128];
          strncpy(textureIdBuffer, i.name.c_str(), sizeof(textureIdBuffer));
          textureIdBuffer[sizeof(textureIdBuffer) - 1] = '\0';

          if (ImGui::InputText("textureId",
                               textureIdBuffer,
                               sizeof(textureIdBuffer),
                               ImGuiInputTextFlags_EnterReturnsTrue))
          {
            if (ImGui::IsItemDeactivatedAfterEdit())
              i.name = std::string(textureIdBuffer);
          }
          char gtextureIdBuffer[128];
          strncpy(gtextureIdBuffer, i.group.c_str(), sizeof(gtextureIdBuffer));
          gtextureIdBuffer[sizeof(gtextureIdBuffer) - 1] = '\0';

          if (ImGui::InputText("Group",
                               gtextureIdBuffer,
                               sizeof(gtextureIdBuffer),
                               ImGuiInputTextFlags_EnterReturnsTrue))
          {
            if (ImGui::IsItemDeactivatedAfterEdit())
              i.group = std::string(gtextureIdBuffer);
          }

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Identification" + std::to_string(entity)).c_str()))
          {
            _ecs.removeComponent<Identification>(entity);
          }

          ImGui::Separator();
        }
      }

      if (_ecs.hasComponent<Transform>(entity))
      {
        if (ImGui::CollapsingHeader("Transform", header))
        {
          auto& t = _ecs.getComponent<Transform>(entity);
          ImGui::DragFloat2("Position", &t.position.x, 1.0f);
          ImGui::DragFloat("Rotation", &t.rotation, 1.0f);

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Transform" + std::to_string(entity)).c_str()))
          {
            _ecs.removeComponent<Transform>(entity);
          }
          ImGui::Separator();
        }
      }

      if (_ecs.hasComponent<RigidBody>(entity))
      {
        if (ImGui::CollapsingHeader("RigidBody", header))
        {
          auto& rb = _ecs.getComponent<RigidBody>(entity);
          ImGui::DragFloat("Mass", &rb.mass, 1.0f);
          ImGui::DragFloat2("Velocity", &rb.velocity.x, 1.0f);

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##RigidBody" + std::to_string(entity)).c_str()))
          {
            _ecs.removeComponent<RigidBody>(entity);
          }
          ImGui::Separator();
        }
      }

      if (_ecs.hasComponent<Force>(entity))
      {
        if (ImGui::CollapsingHeader("Force", header))
        {
          auto& f = _ecs.getComponent<Force>(entity);
          ImGui::DragFloat2("Force", &f.vector.x, 1.0f);

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Force" + std::to_string(entity)).c_str()))
          {
            _ecs.removeComponent<Force>(entity);
          }
          ImGui::Separator();
        }
      }

      if (_ecs.hasComponent<Collider>(entity))
      {
        if (ImGui::CollapsingHeader("Collider", header))
        {
          auto& c = _ecs.getComponent<Collider>(entity);
          ImGui::Checkbox("Static", &c.isStatic);
          ImGui::DragFloat2("Size##Collider", &c.size.x, 1.0f);
          ImGui::DragFloat2("Offset##Collider", &c.offset.x, 1.0f);

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Collider" + std::to_string(entity)).c_str()))
          {
            _ecs.removeComponent<Collider>(entity);
          }
          ImGui::Separator();
        }
      }

      if (_ecs.hasComponent<Renderable>(entity))
      {
        if (ImGui::CollapsingHeader("Renderable", header))
        {
          auto& r = _ecs.getComponent<Renderable>(entity);
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
            _ecs.removeComponent<Renderable>(entity);
          }
          ImGui::Separator();
        }
      }

      if (_ecs.hasComponent<FollowCamera>(entity))
      {
        if (ImGui::CollapsingHeader("FollowCamera", header))
        {
          auto& cam = _ecs.getComponent<FollowCamera>(entity);
          ImGui::Checkbox("_active", &cam.isActive);
          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##FollowCamera" + std::to_string(entity)).c_str()))
          {
            _ecs.removeComponent<FollowCamera>(entity);
          }
          ImGui::Separator();
        }
      }

      if (_ecs.hasComponent<Sprite>(entity))
      {
        auto& sprite = _ecs.getComponent<Sprite>(entity);

        if (ImGui::CollapsingHeader("Sprite", header))
        {
          static char   textureIdBuffer[128] = "";
          static Entity lastEntity           = INVALID_ENTITY;
          if (lastEntity != entity)
          {
            strncpy(textureIdBuffer, sprite.textureId.c_str(), sizeof(textureIdBuffer));
            textureIdBuffer[sizeof(textureIdBuffer) - 1] = '\0';
            lastEntity                                   = entity;
          }

          if (ImGui::InputText("textureId##Sprite",
                               textureIdBuffer,
                               sizeof(textureIdBuffer),
                               ImGuiInputTextFlags_EnterReturnsTrue))
          {
            sprite.textureId = textureIdBuffer;
          }

          ImGui::DragInt4("Src Rect##Sprite", (int*) &sprite.srcRect);
          ImGui::DragFloat("Scale##Sprite", &sprite.scale, 0.01f, 0.1f);
          ImGui::DragFloat2("Offset##Sprite", &sprite.offset.x);
          ImGui::InputInt("Layer", &sprite.layer);

          const char* flipLabels[] = { "none", "horizontal", "vertical" };
          int         flipIndex    = 0;
          if (sprite.flip == "none")
            flipIndex = 0;
          else if (sprite.flip == "horizontal")
            flipIndex = 1;
          else if (sprite.flip == "vertical")
            flipIndex = 2;

          if (ImGui::Combo("Flip", &flipIndex, flipLabels, IM_ARRAYSIZE(flipLabels)))
          {
            sprite.flip = flipLabels[flipIndex];
          }

          const char* blendLabels[] = { "none", "blend", "add", "mod" };
          int         blendIndex    = 0;
          if (sprite.blendMode == "none")
            blendIndex = 0;
          else if (sprite.blendMode == "blend")
            blendIndex = 1;
          else if (sprite.blendMode == "add")
            blendIndex = 2;
          else if (sprite.blendMode == "mod")
            blendIndex = 3;

          if (ImGui::Combo("Blend Mode", &blendIndex, blendLabels, IM_ARRAYSIZE(blendLabels)))
          {
            sprite.blendMode = blendLabels[blendIndex];
          }

          ImGui::NewLine();
          ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
          if (ImGui::SmallButton(("Remove##Sprite" + std::to_string(entity)).c_str()))
          {
            _ecs.removeComponent<Sprite>(entity);
          }
          ImGui::Separator();
        }
        if (_ecs.hasComponent<SpriteAnimator>(entity))
        {
          if (ImGui::CollapsingHeader("Animation", header))
          {
            drawSpriteAnimatorUI(entity);
            ImGui::NewLine();
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 70);
            if (ImGui::SmallButton(("Remove##SpriteAnimator" + std::to_string(entity)).c_str()))
            {
              _ecs.removeComponent<SpriteAnimator>(entity);
            }
            ImGui::Separator();
          }
        }
      }

      // === Add Component ===
      if (ImGui::Button("Add Component"))
        ImGui::OpenPopup(("AddComponentPopup" + std::to_string(entity)).c_str());

      if (ImGui::BeginPopup(("AddComponentPopup" + std::to_string(entity)).c_str()))
      {
        if (!_ecs.hasComponent<Identification>(entity) && ImGui::MenuItem("Identification"))
          _ecs.addComponent<Identification>(entity, {});

        if (!_ecs.hasComponent<Transform>(entity) && ImGui::MenuItem("Transform"))
          _ecs.addComponent<Transform>(entity, {});

        if (!_ecs.hasComponent<RigidBody>(entity) && ImGui::MenuItem("RigidBody"))
          _ecs.addComponent<RigidBody>(entity, {});

        if (!_ecs.hasComponent<Collider>(entity) && ImGui::MenuItem("Collider"))
          _ecs.addComponent<Collider>(entity, {});

        if (!_ecs.hasComponent<Force>(entity) && ImGui::MenuItem("Force"))
          _ecs.addComponent<Force>(entity, {});

        if (!_ecs.hasComponent<Renderable>(entity) && ImGui::MenuItem("Renderable"))
          _ecs.addComponent<Renderable>(entity, {});

        if (!_ecs.hasComponent<FollowCamera>(entity) && ImGui::MenuItem("FollowCamera"))
          _ecs.addComponent<FollowCamera>(entity, {});

        if (!_ecs.hasComponent<Sprite>(entity) && ImGui::MenuItem("Sprite"))
          _ecs.addComponent<Sprite>(entity, {});
        if (!_ecs.hasComponent<SpriteAnimator>(entity) && ImGui::MenuItem("Animation"))
          _ecs.addComponent<SpriteAnimator>(entity, {});

        ImGui::EndPopup();
      }

      ImGui::TreePop();
    }
    ImGui::Separator();
  }

  ImGui::End();

  for (Entity e : destroyQueue)
    _ecs.destroyEntity(e);
  destroyQueue.clear();
}

void
Editor::renderControls()
{
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 0.0f));

  ImGui::Begin("##Toolbar",
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
    _world.getSceneManager().reset(_world.getECS());
  }

  ImGui::SameLine();
  ImGui::Separator();
  ImGui::SameLine();

  static char sceneName[64] = "";
  if (ImGui::Button("Save"))
  {
    strcpy(sceneName, "");
    ImGui::OpenPopup("Save Scene As");
  }

  if (ImGui::BeginPopupModal("Save Scene As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    ImGui::InputText("textureId", sceneName, IM_ARRAYSIZE(sceneName));

    if (ImGui::Button("Save", ImVec2(120, 0)))
    {
      if (strlen(sceneName) > 0)
      {
        JSONSerializer serializer;
        serializer.AddKeyValuePair("name", sceneName);
        serializer.StartArray("entities");

        auto& _ecs = _world.getECS();

        for (Entity entity : _ecs.getEntities())
        {
          serializer.StartNewObject();

          if (_ecs.hasComponent<Transform>(entity))
            ComponentSerializer::Serialize(serializer, _ecs.getComponent<Transform>(entity));
          if (_ecs.hasComponent<RigidBody>(entity))
            ComponentSerializer::Serialize(serializer, _ecs.getComponent<RigidBody>(entity));
          if (_ecs.hasComponent<Force>(entity))
            ComponentSerializer::Serialize(serializer, _ecs.getComponent<Force>(entity));
          if (_ecs.hasComponent<Collider>(entity))
            ComponentSerializer::Serialize(serializer, _ecs.getComponent<Collider>(entity));
          if (_ecs.hasComponent<FollowCamera>(entity))
            ComponentSerializer::Serialize(serializer, _ecs.getComponent<FollowCamera>(entity));
          if (_ecs.hasComponent<Sprite>(entity))
            ComponentSerializer::Serialize(serializer, _ecs.getComponent<Sprite>(entity));
          if (_ecs.hasComponent<Renderable>(entity))
            ComponentSerializer::Serialize(serializer, _ecs.getComponent<Renderable>(entity));
          if (_ecs.hasComponent<Identification>(entity))
            ComponentSerializer::Serialize(serializer, _ecs.getComponent<Identification>(entity));
          if (_ecs.hasComponent<SpriteAnimator>(entity))
            ComponentSerializer::Serialize(serializer, _ecs.getComponent<SpriteAnimator>(entity));

          serializer.EndObject();
        }

        serializer.EndObject();

        std::string filePath = std::string("scenes/") + sceneName + ".json";
        serializer.saveToFile(filePath);

        ImGui::CloseCurrentPopup();
      }
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel", ImVec2(120, 0)))
    {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  ImGui::End();
  ImGui::PopStyleVar(2);
}

void
Editor::drawSpriteAnimatorUI(Entity& e)
{
  auto& ecs      = _world.getECS();
  auto& animator = ecs.getComponent<SpriteAnimator>(e);

  if (ImGui::BeginCombo("Current Animation", animator.currentAnim.c_str()))
  {
    for (const auto& [name, _] : animator.animations)
    {
      bool isSelected = (name == animator.currentAnim);
      if (ImGui::Selectable(name.c_str(), isSelected))
      {
        playAnimation(animator, name);
      }
      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  bool hasValidAnim = animator.animations.find(animator.currentAnim) != animator.animations.end();

  if (!hasValidAnim && !animator.animations.empty())
  {
    animator.currentAnim = animator.animations.begin()->first;
    hasValidAnim         = true;
  }

  if (hasValidAnim)
  {
    auto& animation = animator.animations[animator.currentAnim];
    ImGui::InputInt("Animation Speed (ms/frame)", &animation.speed);
    ImGui::SliderInt(
        "Frame", &animator.currentFrame, 0, std::max(0, (int) animation.frames.size() - 1));

    if (ImGui::Button("Restart Animation"))
    {
      animator.currentFrame = 0;
      animator.timer        = 0.0f;
    }

    ImGui::Separator();
    ImGui::Text("Frames");

    auto&       frames = animation.frames;
    ImTextureID texID  = (ImTextureID) _ctx.texture->getTexture(animation.textureId);

    for (size_t i = 0; i < frames.size(); ++i)
    {
      ImGui::PushID((int) i);
      if (ImGui::TreeNode(("Frame " + std::to_string(i)).c_str()))
      {
        ImGui::InputInt4("Rect", (int*) &frames[i].rect);
        ImGui::DragFloat2("Offset", &frames[i].offset.x, 0.1f);

        if (texID)
        {
          float previewSize = 64.0f;
          int   texW = 1, texH = 1;
          SDL_QueryTexture((SDL_Texture*) texID, nullptr, nullptr, &texW, &texH);

          ImVec2 uv0 = { (float) frames[i].rect.x / texW, (float) frames[i].rect.y / texH };
          ImVec2 uv1 = { (float) (frames[i].rect.x + frames[i].rect.w) / texW,
                         (float) (frames[i].rect.y + frames[i].rect.h) / texH };

          ImGui::Image(texID, ImVec2(previewSize, previewSize), uv0, uv1);
        }

        if (ImGui::Button("Remove Frame"))
        {
          frames.erase(frames.begin() + i);
          ImGui::TreePop();
          ImGui::PopID();
          break;
        }

        ImGui::TreePop();
      }
      ImGui::PopID();
    }

    if (ImGui::Button("Add Frame"))
    {
      frames.emplace_back(SDL_Rect{ 0, 0, 16, 16 }, Vector2D{});
    }
  }
  else
  {
    ImGui::TextColored(ImVec4(1, 0.6f, 0.2f, 1), "No animation selected.");
    ImGui::Text("Create one below to get started.");
  }

  ImGui::Separator();

  static char newAnimName[64]  = {};
  static char newTextureId[64] = {};

  ImGui::InputText("Animation Name", newAnimName, IM_ARRAYSIZE(newAnimName));
  ImGui::InputText("Texture ID", newTextureId, IM_ARRAYSIZE(newTextureId));

  if (ImGui::Button("Create Animation") && strlen(newAnimName) > 0 && strlen(newTextureId) > 0)
  {
    animator.animations[newAnimName] = Animation({}, 200, newTextureId);
    animator.currentAnim             = newAnimName;

    newAnimName[0]  = '\0';
    newTextureId[0] = '\0';
  }

  ImGui::BeginDisabled(!hasValidAnim);
  if (ImGui::Button("Delete Current Animation") && animator.animations.size() > 0)
  {
    animator.animations.erase(animator.currentAnim);
    if (!animator.animations.empty())
      animator.currentAnim = animator.animations.begin()->first;
    else
    {
      animator.currentAnim.clear();
      animator.currentFrame = 0;
      animator.timer        = 0.0f;
    }
  }
  ImGui::EndDisabled();
}

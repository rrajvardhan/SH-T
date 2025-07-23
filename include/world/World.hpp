#pragma once

#include "Animation.hpp"
#include "Camera.hpp"
#include "Camera2D.hpp"
#include "Collision.hpp"
#include "DebugDraw.hpp"
#include "EventBus.hpp"
#include "GlobalScriptSystem.hpp"
#include "Overseer.hpp"
#include "Physics.hpp"
#include "Provider.hpp"
#include "Renderable.hpp"
#include "SceneManager.hpp"
#include "ServiceContext.hpp"
#include "SpriteRender.hpp"
#include <memory>

class World
{
public:
  World(ServiceContext& ctx);
  ~World();

  bool init();
  void registerMainSystems();
  void update();
  void render();

  WorldProvider&      getProvider() { return _provider; }
  Overseer&           getECS() { return _ecs; }
  Camera2D&           getCamera() { return _camera; }
  EventBus&           getEventBus() { return _eventbus; }
  GlobalScriptSystem& getScriptSystem() { return *_globalScript; }

  void clearAll()
  {
    std::vector<Entity> toDestroy;
    for (Entity e : _ecs.getEntities())
      toDestroy.push_back(e);

    for (Entity e : toDestroy)
      _ecs.destroyEntity(e);
  }

  bool isDebug = false;

private:
  Overseer        _ecs;
  Camera2D        _camera;
  ServiceContext& _ctx;
  WorldProvider   _provider;
  EventBus        _eventbus;

  // Scene Manager
  std::unique_ptr<SceneManager> _sceneManager;

  // Lua scripting System
  std::unique_ptr<GlobalScriptSystem> _globalScript;

  // Main Systems
  std::shared_ptr<PhysicsSystem>      physicsSystem;
  std::shared_ptr<RenderSystem>       renderSystem;
  std::shared_ptr<DebugDrawSystem>    debugDrawSystem;
  std::shared_ptr<CollisionSystem>    collisionSystem;
  std::shared_ptr<FollowCameraSystem> cameraSystem;
  std::shared_ptr<SpriteSystem>       spriteRenderSystem;
  std::shared_ptr<AnimationSystem>    spriteAnimationSystem;
};

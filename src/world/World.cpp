#include "Animation.hpp"
#include "Camera.hpp"
#include "CameraComponents.hpp"
#include "Collision.hpp"
#include "CollisionComponents.hpp"
#include "DebugDraw.hpp"
#include "LuaBindings.hpp"
#include "Overseer.hpp"
#include "Physics.hpp"
#include "PhysicsComponents.hpp"
#include "Renderable.hpp"
#include "RenderableComponents.hpp"
#include "SpriteComponents.hpp"
#include "SpriteRender.hpp"
#include "Types.hpp"
#include "UtilComponents.hpp"
#include "World.hpp"

World::World(ServiceContext& ctx) : _camera(), _ctx(ctx), _provider(ctx, _camera, _eventbus)
{
}

World::~World()
{
}

bool
World::init()
{
  _ecs.init();

  _ecs.registerComponent<Collider>();
  _ecs.registerComponent<Transform>();
  _ecs.registerComponent<RigidBody>();
  _ecs.registerComponent<Force>();
  _ecs.registerComponent<Renderable>();
  _ecs.registerComponent<FollowCamera>();
  _ecs.registerComponent<Sprite>();
  _ecs.registerComponent<SpriteAnimator>();
  _ecs.registerComponent<Identification>();

  _sceneManager = std::make_unique<SceneManager>(_ecs);
  _globalScript = std::make_unique<GlobalScriptSystem>();

  // Core Systems
  registerMainSystems();

  // Lua Bindings
  auto& lua = _globalScript->getLuaState();
  // Components
  LuaBindings::bindECSCore(lua, _ecs);
  LuaBindings::bindECSComponent<Transform>(lua, _ecs, "transform");
  LuaBindings::bindECSComponent<Collider>(lua, _ecs, "collider");
  LuaBindings::bindECSComponent<RigidBody>(lua, _ecs, "rigidbody");
  LuaBindings::bindECSComponent<Force>(lua, _ecs, "force");
  LuaBindings::bindECSComponent<FollowCamera>(lua, _ecs, "follow_camera");
  LuaBindings::bindECSComponent<Identification>(lua, _ecs, "id");

  LuaBindings::bindEntityLookup(lua, _ecs);
  // Services
  LuaBindings::bindKeyConstants(lua);
  LuaBindings::bindInput(lua, *_provider.service.input);
  LuaBindings::bindTimer(lua, *_provider.service.timer);
  LuaBindings::bindAudio(lua, *_provider.service.audio);

  LuaBindings::bindCamera2D(lua, getCamera());
  LuaBindings::bindSceneManager(lua, *_sceneManager);
  LuaBindings::bindAnimation(lua, _ecs);

  // ECS Event Bus
  _eventbus.subscribe(_globalScript.get(), &GlobalScriptSystem::onCollision);

  return true;
}

void
World::registerMainSystems()
{
  physicsSystem = _ecs.registerSystem<PhysicsSystem>();
  {
    Signature sig;
    sig.set(_ecs.getComponentType<Transform>());
    sig.set(_ecs.getComponentType<RigidBody>());
    _ecs.setSystemSignature<PhysicsSystem>(sig);
  }

  renderSystem = _ecs.registerSystem<RenderSystem>();
  {
    Signature sig;
    sig.set(_ecs.getComponentType<Renderable>());
    sig.set(_ecs.getComponentType<Transform>());
    _ecs.setSystemSignature<RenderSystem>(sig);
  }

  debugDrawSystem = _ecs.registerSystem<DebugDrawSystem>();
  {
    Signature sig;
    sig.set(_ecs.getComponentType<Transform>());
    _ecs.setSystemSignature<DebugDrawSystem>(sig);
  }

  collisionSystem = _ecs.registerSystem<CollisionSystem>();
  {
    Signature sig;
    sig.set(_ecs.getComponentType<Transform>());
    sig.set(_ecs.getComponentType<Collider>());
    _ecs.setSystemSignature<CollisionSystem>(sig);
  }

  cameraSystem = _ecs.registerSystem<FollowCameraSystem>();
  {
    Signature sig;
    sig.set(_ecs.getComponentType<FollowCamera>());
    sig.set(_ecs.getComponentType<Transform>());
    _ecs.setSystemSignature<FollowCameraSystem>(sig);
  }

  spriteRenderSystem = _ecs.registerSystem<SpriteSystem>();
  {
    Signature sig;
    sig.set(_ecs.getComponentType<Sprite>());
    sig.set(_ecs.getComponentType<Transform>());
    _ecs.setSystemSignature<SpriteSystem>(sig);
  }

  spriteAnimationSystem = _ecs.registerSystem<AnimationSystem>();
  {
    Signature sig;
    sig.set(_ecs.getComponentType<Sprite>());
    sig.set(_ecs.getComponentType<SpriteAnimator>());
    _ecs.setSystemSignature<AnimationSystem>(sig);
  }
}

void
World::update()
{
  physicsSystem->update(_ecs, _provider);
  collisionSystem->update(_ecs, _provider);
  spriteAnimationSystem->update(_ecs, _provider);
  cameraSystem->update(_ecs, _provider);

  if (_globalScript)
    _globalScript->update();
}

void
World::render()
{
  spriteRenderSystem->update(_ecs, _provider);
  renderSystem->update(_ecs, _provider);

  if (isDebug)
    debugDrawSystem->update(_ecs, _provider);
}

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
  _sceneManager = std::make_unique<SceneManager>();
  _globalScript = std::make_unique<GlobalScriptSystem>();
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

  registerMainSystems();

  _globalScript->bind(_ecs);
  auto& lua = _globalScript->getLuaState();

  LuaBindings::bindVector2D(lua);
  LuaBindings::bindTransform(lua, _ecs);
  LuaBindings::bindFollowCamera(lua, _ecs);
  LuaBindings::bindCollider(lua, _ecs);
  LuaBindings::bindRigidBody(lua, _ecs);
  LuaBindings::bindKeyConstants(lua);
  LuaBindings::bindInput(lua, *_provider.service.input);
  LuaBindings::bindTimer(lua, *_provider.service.timer);
  LuaBindings::bindAudio(lua, *_provider.service.audio);
  LuaBindings::bindCamera2D(lua, getCamera());
  LuaBindings::bindSceneManager(lua, *_sceneManager, _ecs);

  _globalScript->loadScript("scripts/main.lua");
  _sceneManager->loadScene("scenes/test.json", _ecs);

  _eventbus.subscribe(_globalScript.get(), &GlobalScriptSystem::onCollision);

  Entity player = _ecs.createEntity();
  _ecs.addComponent(player, Transform{ { 0.0f, 100.0f } });
  _ecs.addComponent(player, RigidBody{ { 0.0f, 0.0f }, { 0.0f, 0.0f }, 1.0f });
  _ecs.addComponent(player, Force{ { 0.0f, 2000.0f } });
  _ecs.addComponent(player, Collider{ { 90.0f, 90.0f } });
  _ecs.addComponent(player,
                    Sprite{ .textureId = "bird", .srcRect = { 0, 16, 16, 16 }, .scale = 5.0f });
  _ecs.addComponent(player, Identification{ "bird", "bird" });
  _ecs.addComponent(player, FollowCamera{ true });
  SpriteAnimator animator;
  animator.animations["idle"] = Animation(
      {
          AnimationFrame({ 16 * 0, 16, 16, 16 }),
          AnimationFrame({ 16 * 1, 16, 16, 16 }),
          AnimationFrame({ 16 * 2, 16, 16, 16 }),
          AnimationFrame({ 16 * 3, 16, 16, 16 }),
          AnimationFrame({ 16 * 4, 16, 16, 16 }),
          AnimationFrame({ 16 * 5, 16, 16, 16 }),
          AnimationFrame({ 16 * 6, 16, 16, 16 }),
          AnimationFrame({ 16 * 7, 16, 16, 16 }),
      },
      150,
      "bird");

  playAnimation(animator, "idle");

  _ecs.addComponent(player, animator);

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

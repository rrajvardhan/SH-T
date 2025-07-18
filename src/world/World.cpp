#include "World.hpp"
#include "Animation.hpp"
#include "Camera.hpp"
#include "CameraComponents.hpp"
#include "Collision.hpp"
#include "CollisionComponents.hpp"
#include "DebugDraw.hpp"
#include "GlobalScriptSystem.hpp"
#include "LuaBindings.hpp"
#include "MovingPlatform.hpp"
#include "Objects.hpp"
#include "Overseer.hpp"
#include "Physics.hpp"
#include "PhysicsComponents.hpp"
#include "Renderable.hpp"
#include "RenderableComponents.hpp"
#include "SpriteComponents.hpp"
#include "SpriteRender.hpp"
#include "Types.hpp"
#include "UtilComponents.hpp"
#include <string>

World::World(ServiceContext& ctx) : _camera(), _ctx(ctx), _provider(ctx, _camera, _eventbus)
{
}

World::~World()
{
}

Entity player;
Entity movingPlatform;

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
  _ecs.registerComponent<PlatformerCharacter>();
  _ecs.registerComponent<MovingPlatform>();

  registerMainSystems();

  _globalScript = std::make_unique<GlobalScriptSystem>();
  _globalScript->bind(_ecs);

  auto& lua = _globalScript->getLuaState();

  LuaBindings::bindVector2D(lua);
  LuaBindings::bindTransform(lua, _ecs);
  LuaBindings::bindFollowCamera(lua, _ecs);
  LuaBindings::bindCollider(lua, _ecs);
  LuaBindings::bindRigidBody(lua, _ecs);

  LuaBindings::bindKeyConstants(lua);
  LuaBindings::bindInput(lua, *_provider.service.input);

  _globalScript->loadScript("scripts/main.lua");

  _eventbus.subscribe(_globalScript.get(), &GlobalScriptSystem::onAnyEvent);

  //////////////////////////////////////////////////////////////////

  player = _ecs.createEntity();
  _ecs.addComponent(player, Transform{ { 0.0f, 100.0f } });
  _ecs.addComponent(player, RigidBody{ { 0.0f, 0.0f }, { 0.0f, 0.0f }, 1.0f });
  _ecs.addComponent(player, Force{ { 0.0f, 2000.0f } });
  _ecs.addComponent(player, Collider{ { 90.0f, 90.0f } });
  _ecs.addComponent(player, Sprite{ .name = "bird", .srcRect = { 0, 16, 16, 16 }, .scale = 5.0f });

  SpriteAnimator animator;
  animator.animations["idle"] = Animation(
      {
          { { 16 * 0, 16, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 1, 16, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 2, 16, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 3, 16, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 4, 16, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 5, 16, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 6, 16, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 7, 16, 16, 16 }, { 0.0f, 0.0f } },
      },
      150);

  animator.currentAnim = "idle";
  _ecs.addComponent(player, animator);
  _ecs.addComponent(player, Identification{ "bird", "bird" });

  // Ground
  Entity ground = _ecs.createEntity();
  _ecs.addComponent(ground, Transform{ { 0.0f, 500.0f } });
  _ecs.addComponent(ground, Collider{ { 1600.0f, 16.0f }, { 0.0f, 0.0f }, true });
  _ecs.addComponent(ground, Renderable{ { 1600.0f, 16.0f }, { 0, 255, 255, 255 } });

  // Moving platform
  movingPlatform = _ecs.createEntity();
  _ecs.addComponent(movingPlatform, Transform{ { 0.0f, 400.0f } });
  _ecs.addComponent(movingPlatform, Collider{ { 100.0f, 16.0f }, { 0.0f, 0.0f }, true });
  _ecs.addComponent(movingPlatform, Renderable{ { 100.0f, 16.0f }, { 255, 128, 0, 255 } });
  _ecs.addComponent(movingPlatform, Identification{ "MovingPlatform" });
  _ecs.addComponent(movingPlatform, MovingPlatform{});

  return true;
}

void
World::registerMainSystems()
{
  movingPlatformSystem = _ecs.registerSystem<MovingPlatformSystem>();
  {
    Signature sig;
    sig.set(_ecs.getComponentType<Transform>());
    sig.set(_ecs.getComponentType<MovingPlatform>());
    _ecs.setSystemSignature<MovingPlatformSystem>(sig);
  }

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
  movingPlatformSystem->update(_ecs, _provider);

  physicsSystem->update(_ecs, _provider);
  collisionSystem->update(_ecs, _provider);
  spriteAnimationSystem->update(_ecs, _provider);
  cameraSystem->update(_ecs, _provider);

  if (_globalScript)
    _globalScript->update(_ctx.timer->getDeltaTime());
}

void
World::render()
{
  spriteRenderSystem->update(_ecs, _provider);
  renderSystem->update(_ecs, _provider);

  if (isDebug)
    debugDrawSystem->update(_ecs, _provider);
}

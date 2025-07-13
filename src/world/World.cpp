#include "Animation.hpp"
#include "Camera.hpp"
#include "CameraComponents.hpp"
#include "Character.hpp"
#include "Collision.hpp"
#include "CollisionComponents.hpp"
#include "DebugDraw.hpp"
#include "EventBus.hpp"
#include "GlobalScriptSystem.hpp"
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
#include "World.hpp"
#include <SDL2/SDL_scancode.h>

World::World(ServiceContext& ctx) : _camera(), _ctx(ctx), _provider(ctx, _camera)
{
  ecs.init();
  ecs.registerComponent<Transform>();
  ecs.registerComponent<RigidBody>();
  ecs.registerComponent<Force>();
  ecs.registerComponent<Renderable>();
  ecs.registerComponent<Collider>();
  ecs.registerComponent<FollowCamera>();
  ecs.registerComponent<Sprite>();
  ecs.registerComponent<SpriteAnimator>();
  ecs.registerComponent<Identification>();
  ecs.registerComponent<PlatformerCharacter>();
  ecs.registerComponent<MovingPlatform>();
}

World::~World()
{
}

EventBus eventBus;

Entity player;
Entity movingPlatform;

bool
World::init()
{

  _lua.open_libraries(
      sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::package, sol::lib::string);

  _globalScript = new GlobalScriptSystem(_lua, "main.lua");

  registerSystem<PlatformerCharacterSystem>("Character Movement", 8);
  Signature chsig;
  chsig.set(ecs.getComponentType<RigidBody>());
  chsig.set(ecs.getComponentType<PlatformerCharacter>());
  ecs.setSystemSignature<PlatformerCharacterSystem>(chsig);

  registerSystem<MovingPlatformSystem>("Platfrom Movement", 7);
  Signature msig;
  msig.set(ecs.getComponentType<Transform>());
  msig.set(ecs.getComponentType<MovingPlatform>());
  ecs.setSystemSignature<MovingPlatformSystem>(msig);

  registerSystem<PhysicsSystem>("Physics", 11);
  Signature psig;
  psig.set(ecs.getComponentType<Transform>());
  psig.set(ecs.getComponentType<RigidBody>());
  ecs.setSystemSignature<PhysicsSystem>(psig);

  registerSystem<RenderSystem>("Renderable", 20, "render");
  Signature rsig;
  rsig.set(ecs.getComponentType<Renderable>());
  ecs.setSystemSignature<RenderSystem>(rsig);

  registerSystem<DebugDrawSystem>("Debug", 22, "render");
  Signature tdSig;
  tdSig.set(ecs.getComponentType<Transform>());
  ecs.setSystemSignature<DebugDrawSystem>(tdSig);

  registerSystem<CollisionSystem>("Collision", 12);
  Signature csig;
  csig.set(ecs.getComponentType<Transform>());
  csig.set(ecs.getComponentType<Collider>());
  ecs.setSystemSignature<CollisionSystem>(csig);

  registerSystem<FollowCameraSystem>("Camera", 13);
  Signature css;
  css.set(ecs.getComponentType<FollowCamera>());
  ecs.setSystemSignature<FollowCameraSystem>(css);

  registerSystem<SpriteRenderSystem>("Sprite", 21, "render");
  Signature ssig;
  ssig.set(ecs.getComponentType<Sprite>());
  ssig.set(ecs.getComponentType<Transform>());
  ecs.setSystemSignature<SpriteRenderSystem>(ssig);

  registerSystem<SpriteAnimationSystem>("Animation", 10);
  Signature animSig;
  animSig.set(ecs.getComponentType<Sprite>());
  animSig.set(ecs.getComponentType<SpriteAnimator>());
  ecs.setSystemSignature<SpriteAnimationSystem>(animSig);

  _provider.service.texture->addTexture("test", "assets/char_spritesheet.png");
  SDL_Texture* test = _provider.service.texture->getTexture("test");

  player = ecs.createEntity();
  ecs.addComponent(player, Transform{ { 0.0f, 100.0f } });
  ecs.addComponent(player, RigidBody{ { 0.0f, 0.0f }, { 0.0f, 0.0f }, 1.0f });
  ecs.addComponent(player, Force{ { 0.0f, 2000.0f } });
  ecs.addComponent(player, Collider{ { 90.0f, 90.0f } });
  ecs.addComponent(player, Sprite{ .texture = test, .srcRect = { 16, 16, 16, 16 }, .scale = 5.0f });
  ecs.addComponent(player, PlatformerCharacter{});

  SpriteAnimator animator;
  animator.animations["idle"] = Animation(
      {
          { { 16 * 1, 16 * 1, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 2, 16 * 1, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 3, 16 * 1, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 4, 16 * 1, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 5, 16 * 1, 16, 16 }, { 0.0f, 0.0f } },
          { { 16 * 6, 16 * 1, 16, 16 }, { 0.0f, 0.0f } },
      },
      150);

  animator.currentAnim = "idle";
  ecs.addComponent(player, animator);
  ecs.addComponent(player, Identification{ "Player", "Player" });

  // Ground
  Entity ground = ecs.createEntity();
  ecs.addComponent(ground, Transform{ { 0.0f, 500.0f } });
  ecs.addComponent(ground, Collider{ { 1600.0f, 16.0f }, { 0.0f, 0.0f }, true });
  ecs.addComponent(ground, Renderable{ { 1600.0f, 16.0f }, { 0, 255, 255, 255 } });

  // Moving platform
  movingPlatform = ecs.createEntity();
  ecs.addComponent(movingPlatform, Transform{ { 0.0f, 400.0f } });
  ecs.addComponent(movingPlatform, Collider{ { 100.0f, 16.0f }, { 0.0f, 0.0f }, true });
  ecs.addComponent(movingPlatform, Renderable{ { 100.0f, 16.0f }, { 255, 128, 0, 255 } });
  ecs.addComponent(movingPlatform, Identification{ "MovingPlatform", "Test" });
  ecs.addComponent(movingPlatform, MovingPlatform{});

  return true;
}

void
World::render()
{
  for (auto& system : _systems)
    if (system.enabled && system.phase == "render")
      system.func(ecs, _provider);
}

void
World::update()
{
  float dt = _ctx.timer->getDeltaTime();
  _globalScript->update(dt);

  for (auto& system : _systems)
    if (system.enabled && system.phase == "update")
      system.func(ecs, _provider);
}

#include "Animation.hpp"
#include "Camera.hpp"
#include "CameraComponents.hpp"
#include "Collision.hpp"
#include "CollisionComponents.hpp"
#include "DebugDraw.hpp"
#include "EventBus.hpp"
#include "Log.hpp"
#include "Overseer.hpp"
#include "Physics.hpp"
#include "PhysicsComponents.hpp"
#include "Renderable.hpp"
#include "RenderableComponents.hpp"
#include "SpriteComponents.hpp"
#include "SpriteRender.hpp"
#include "Types.hpp"
#include "UtilComponents.hpp"
#include "Vector2D.hpp"
#include "World.hpp"
#include <SDL2/SDL_scancode.h>

World::World(ServiceContext& ctx) : _cameraOffset(), _ctx(ctx), _provider(ctx, _cameraOffset)
{
}

World::~World()
{
}

std::shared_ptr<PhysicsSystem>         physics;
std::shared_ptr<RenderSystem>          renderables;
std::shared_ptr<DebugDrawSystem>       debugDraw;
std::shared_ptr<CollisionSystem>       collisionSystem;
std::shared_ptr<SpriteRenderSystem>    spriteRenderer;
std::shared_ptr<SpriteAnimationSystem> spriteAnimator;
std::shared_ptr<FollowCameraSystem>    cs;

EventBus eventBus;

Entity c;
Entity player;

bool
World::init()
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

  physics = ecs.registerSystem<PhysicsSystem>();
  Signature psig;
  psig.set(ecs.getComponentType<Transform>());
  psig.set(ecs.getComponentType<RigidBody>());
  ecs.setSystemSignature<PhysicsSystem>(psig);

  renderables = ecs.registerSystem<RenderSystem>();
  Signature rsig;
  rsig.set(ecs.getComponentType<Renderable>());
  ecs.setSystemSignature<RenderSystem>(rsig);

  debugDraw = ecs.registerSystem<DebugDrawSystem>();
  Signature tdSig;
  tdSig.set(ecs.getComponentType<Transform>());
  ecs.setSystemSignature<DebugDrawSystem>(tdSig);

  collisionSystem = ecs.registerSystem<CollisionSystem>();
  Signature csig;
  csig.set(ecs.getComponentType<Transform>());
  csig.set(ecs.getComponentType<Collider>());
  ecs.setSystemSignature<CollisionSystem>(csig);

  cs = ecs.registerSystem<FollowCameraSystem>();
  Signature css;
  css.set(ecs.getComponentType<FollowCamera>());
  ecs.setSystemSignature<FollowCameraSystem>(css);

  spriteRenderer = ecs.registerSystem<SpriteRenderSystem>();
  Signature ssig;
  ssig.set(ecs.getComponentType<Sprite>());
  ssig.set(ecs.getComponentType<Transform>());
  ecs.setSystemSignature<SpriteRenderSystem>(ssig);

  spriteAnimator = ecs.registerSystem<SpriteAnimationSystem>();
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

  // Camera
  c = ecs.createEntity();
  ecs.addComponent(c, FollowCamera{ .target = player, .isActive = true });
  ecs.addComponent(c, Transform{ { 0.0f, 0.0f } });

  LOG_INFO("[World] initialized.");
  return true;
}

void
World::render()
{

  renderables->update(ecs, _provider);
  spriteRenderer->update(ecs, _provider);
  debugDraw->update(ecs, _provider);
}

void
World::update()
{

  auto& input = *_provider.service.input;

  if (ecs.hasComponent<RigidBody>(player))
  {
    auto& rb = ecs.getComponent<RigidBody>(player);

    // Horizontal movement
    if (input.keyDown(SDL_SCANCODE_A))
      rb.velocity.x = -200.0f;
    else if (input.keyDown(SDL_SCANCODE_D))
      rb.velocity.x = 200.0f;
    else
      rb.velocity.x = 0.0f;

    // Jump
    if (input.keyDown(SDL_SCANCODE_SPACE))
      rb.velocity.y = -600.0f; // instant jump
  }

  spriteAnimator->update(ecs, _provider);
  physics->update(ecs, _provider);
  collisionSystem->update(ecs, _provider);
  cs->update(ecs, _provider);
}

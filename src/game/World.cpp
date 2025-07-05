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
std::shared_ptr<CameraSystem>          cs;

EventBus eventBus;

Entity c;      // Camera entity
Entity player; // Player entity

bool
World::init()
{
  ecs.init();
  ecs.registerComponent<Transform>();
  ecs.registerComponent<RigidBody>();
  ecs.registerComponent<Force>();
  ecs.registerComponent<Renderable>();
  ecs.registerComponent<Collider>();
  ecs.registerComponent<Camera>();
  ecs.registerComponent<Sprite>();
  ecs.registerComponent<SpriteAnimator>();

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

  cs = ecs.registerSystem<CameraSystem>();
  Signature css;
  css.set(ecs.getComponentType<Camera>());
  ecs.setSystemSignature<CameraSystem>(css);

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
  _provider.service.texture->addTexture("col", "assets/collisionTex.png");
  SDL_Texture* test = _provider.service.texture->getTexture("test");
  SDL_Texture* col  = _provider.service.texture->getTexture("col");
  // Player

  player = ecs.createEntity();
  ecs.addComponent(player, Transform{ { 0.0f, 200.0f } });
  ecs.addComponent(player, RigidBody{ { 0.0f, 0.0f }, { 0.0f, 0.0f }, 1.0f });
  ecs.addComponent(player, Collider{ { 40.0f, 60.0f }, { 0.0f, 0.0f }, true });

  {
    player = ecs.createEntity();
    ecs.addComponent(player, Transform{ { 0.0f, 100.0f } });
    ecs.addComponent(player, RigidBody{ { 0.0f, 0.0f }, { 0.0f, 0.0f }, 1.0f });
    ecs.addComponent(player, Force{ { 0.0f, 2000.0f } });
    ecs.addComponent(player, Collider{ { 90.0f, 90.0f } });
    ecs.addComponent(player,
                     Sprite{ .texture = test, .srcRect = { 16, 16, 16, 16 }, .scale = 5.0f });

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
  }

  {
    Entity player = ecs.createEntity();
    ecs.addComponent(player, Transform{ { 0.0f, 0.0f } });
    ecs.addComponent(player, RigidBody{ { 0.0f, 0.0f }, { 0.0f, 0.0f }, 1.0f });
    ecs.addComponent(player, Collider{ { 90.0f, 90.0f }, {}, true });
    ecs.addComponent(player, Sprite{ .texture = col, .srcRect = { 0, 0, 16, 16 }, .scale = 5.0f });
  }

  // Ground
  Entity ground = ecs.createEntity();
  ecs.addComponent(ground, Transform{ { 0.0f, 500.0f } });
  ecs.addComponent(ground, Collider{ { 1600.0f, 16.0f }, { 0.0f, 0.0f }, true });

  // Camera
  c = ecs.createEntity();
  ecs.addComponent(c, Camera{});
  ecs.addComponent(c, Transform{ { 0.0f, 0.0f } });

  LOG_INFO("[World] Platformer initialized.");
  return true;
}

void
World::render()
{

  spriteRenderer->update(ecs, _provider);
  renderables->update(ecs, _provider);
  debugDraw->update(ecs, _provider);
}

void
World::update()
{

  spriteAnimator->update(ecs, _provider.service.timer->getDeltaTime());
  physics->update(ecs, _provider.service.timer->getDeltaTime());
  collisionSystem->update(ecs);
  cs->update(ecs, _provider);
}

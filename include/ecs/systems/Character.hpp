#pragma once

#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Provider.hpp"
#include "System.hpp"

class PlatformerCharacterSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {
    auto& input = provider.service.input;

    for (Entity e : _entities)
    {
      if (!ecs.hasComponent<RigidBody>(e))
        continue;

      auto& rb = ecs.getComponent<RigidBody>(e);

      if (input->keyDown(SDL_SCANCODE_A))
        rb.velocity.x = -200.0f;
      else if (input->keyDown(SDL_SCANCODE_D))
        rb.velocity.x = 200.0f;
      else
        rb.velocity.x = 0.0f;

      if (input->keyDown(SDL_SCANCODE_SPACE))
        rb.velocity.y = -600.0f;
    }
  }
};

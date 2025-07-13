
#pragma once

#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Provider.hpp"
#include "System.hpp"

class MovingPlatformSystem : public System
{
public:
  float moveTimer = 0.0f;
  float moveSpeed = 50.0f;
  int   direction = 1;

  void update(Overseer& ecs, WorldProvider provider)
  {
    float dt = provider.service.timer->getDeltaTime();

    for (Entity e : _entities)
    {
      if (!ecs.hasComponent<Transform>(e))
        continue;

      auto& tf = ecs.getComponent<Transform>(e);
      tf.position.x += direction * moveSpeed * dt;

      moveTimer += dt;
      if (moveTimer >= 10.0f)
      {
        direction *= -1;
        moveTimer = 0.0f;
      }
    }
  }
};

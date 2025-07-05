#pragma once

#include "Overseer.hpp"
#include "PhysicsComponents.hpp"

class PhysicsSystem : public System
{
public:
  void update(Overseer& ecs, float dt)
  {
    for (auto entity : _entities)
    {
      auto& rb = ecs.getComponent<RigidBody>(entity);
      auto& tr = ecs.getComponent<Transform>(entity);

      if (ecs.hasComponent<Force>(entity))
      {
        auto& f = ecs.getComponent<Force>(entity);
        rb.acceleration += f.vector / rb.mass;
      }

      rb.velocity += rb.acceleration * dt;
      tr.position += rb.velocity * dt;

      rb.acceleration = { 0, 0 };
    }
  }
};

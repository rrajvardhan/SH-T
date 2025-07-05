#pragma once

#include "CollisionComponents.hpp"
#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "World.hpp"

class DebugDrawSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {
    constexpr float MAX_VELOCITY_LENGTH = 50.0f;
    constexpr float MAX_FORCE_LENGTH    = 50.0f;

    for (auto entity : _entities)
    {
      const auto& tr = ecs.getComponent<Transform>(entity);

      Vector2D screenPos = tr.position - provider.getCameraOffset();
      int      x         = static_cast<int>(screenPos.x);
      int      y         = static_cast<int>(screenPos.y);

      if (ecs.hasComponent<RigidBody>(entity))
      {
        const auto& rb  = ecs.getComponent<RigidBody>(entity);
        Vector2D    vel = rb.velocity;

        float mag = vel.magnitude();
        if (mag > 0.0f)
        {
          Vector2D dir    = vel.normalized();
          float    length = std::min(mag * 10.0f, MAX_VELOCITY_LENGTH);
          int      dx     = static_cast<int>(dir.x * length);
          int      dy     = static_cast<int>(dir.y * length);

          provider.service.graphics->drawLine(x, y, x + dx, y + dy, { 255, 255, 0, 255 });
        }
      }

      if (ecs.hasComponent<Force>(entity))
      {
        const auto& force = ecs.getComponent<Force>(entity);
        Vector2D    f     = force.vector;

        float mag = f.magnitude();
        if (mag > 0.0f)
        {
          Vector2D dir    = f.normalized();
          float    length = std::min(mag * 0.1f, MAX_FORCE_LENGTH);
          int      dx     = static_cast<int>(dir.x * length);
          int      dy     = static_cast<int>(dir.y * length);

          provider.service.graphics->drawLine(x, y, x + dx, y + dy, { 0, 255, 255, 255 });
        }
      }

      if (ecs.hasComponent<Collider>(entity))
      {
        const auto& col      = ecs.getComponent<Collider>(entity);
        Vector2D    halfSize = col.size * 0.5f;
        Vector2D    center   = tr.position + col.offset;
        Vector2D    topLeft  = center - halfSize - provider.getCameraOffset();

        SDL_Color color = col.isStatic ? SDL_Color{ 255, 0, 255, 128 } // Magenta for static
                                       : SDL_Color{ 255, 0, 0, 128 };  // Red for dynamic

        provider.service.graphics->drawFilledRect(static_cast<int>(topLeft.x),
                                                  static_cast<int>(topLeft.y),
                                                  static_cast<int>(col.size.x),
                                                  static_cast<int>(col.size.y),
                                                  color);
      }

      // Origin crosshair
      provider.service.graphics->drawLine(x - 3, y, x + 3, y, { 255, 255, 255, 255 });
      provider.service.graphics->drawLine(x, y - 3, x, y + 3, { 255, 255, 255, 255 });
    }
  }
};

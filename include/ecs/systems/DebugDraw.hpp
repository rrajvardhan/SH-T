#pragma once

#include "CollisionComponents.hpp"
#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Provider.hpp"
#include <SDL2/SDL_blendmode.h>

class DebugDrawSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {
    constexpr float MAX_VELOCITY_LENGTH = 50.0f;
    constexpr float MAX_FORCE_LENGTH    = 50.0f;

    provider.service.graphics->setBlendMode("blend");

    float zoom = provider.camera.getZoom();

    for (auto entity : _entities)
    {
      const auto& tr = ecs.getComponent<Transform>(entity);

      Vector2D screenPos = (tr.position - provider.camera.getOffset()) * zoom;
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
          float    length = std::min(mag * 10.0f, MAX_VELOCITY_LENGTH) * zoom;
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
          float    length = std::min(mag * 0.1f, MAX_FORCE_LENGTH) * zoom;
          int      dx     = static_cast<int>(dir.x * length);
          int      dy     = static_cast<int>(dir.y * length);

          provider.service.graphics->drawLine(x, y, x + dx, y + dy, { 0, 255, 255, 255 });
        }
      }

      if (ecs.hasComponent<Collider>(entity))
      {
        const auto& col = ecs.getComponent<Collider>(entity);

        Vector2D center   = (tr.position + col.offset - provider.camera.getOffset()) * zoom;
        Vector2D halfSize = col.size * 0.5f * zoom;
        Vector2D topLeft  = center - halfSize;

        if (col.isStatic)
        {
          provider.service.graphics->drawFilledRect(static_cast<int>(topLeft.x),
                                                    static_cast<int>(topLeft.y),
                                                    static_cast<int>(col.size.x * zoom),
                                                    static_cast<int>(col.size.y * zoom),
                                                    { 255, 0, 255, 128 }); // Magenta for static
        }
        else
        {
          provider.service.graphics->drawFilledRect(static_cast<int>(topLeft.x),
                                                    static_cast<int>(topLeft.y),
                                                    static_cast<int>(col.size.x * zoom),
                                                    static_cast<int>(col.size.y * zoom),
                                                    { 255, 0, 0, 128 }); // Red for static
        }
      }

      // Origin crosshair
      provider.service.graphics->drawLine(x - 3 * zoom, y, x + 3 * zoom, y, { 255, 255, 255, 255 });
      provider.service.graphics->drawLine(x, y - 3 * zoom, x, y + 3 * zoom, { 255, 255, 255, 255 });
    }
    provider.service.graphics->setBlendMode("none");
  }
};

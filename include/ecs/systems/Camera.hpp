#pragma once

#include "CameraComponents.hpp"
#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Types.hpp"
#include "World.hpp"

class CameraSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider& provider)
  {

    auto& context = provider.service;
    float dt      = context.timer->getDeltaTime();

    for (auto e : _entities)
    {
      if (!ecs.hasComponent<Camera>(e))
        continue;

      auto& camTr  = ecs.getComponent<Transform>(e);
      auto& camera = ecs.getComponent<Camera>(e);

      Vector2D screenCenter{ provider.vw * 0.5f, provider.vh * 0.5f };

      if (camera.target != INVALID_ENTITY && ecs.hasComponent<Transform>(camera.target))
      {
        // Follow target
        Vector2D targetPos = ecs.getComponent<Transform>(camera.target).position;
        camTr.position     = targetPos;
      }
      else
      {
        // Manual WASD movement
        Vector2D move  = { 0.f, 0.f };
        float    speed = 500.0f;

        if (context.input->keyDown(SDL_SCANCODE_W))
          move.y -= 1;
        if (context.input->keyDown(SDL_SCANCODE_S))
          move.y += 1;
        if (context.input->keyDown(SDL_SCANCODE_A))
          move.x -= 1;
        if (context.input->keyDown(SDL_SCANCODE_D))
          move.x += 1;

        camTr.position += move * speed * dt;
      }

      provider.setCameraOffset(camTr.position - screenCenter);
    }
  }
};

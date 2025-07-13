#pragma once

#include "CameraComponents.hpp"
#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Provider.hpp"
#include "Types.hpp"

class FollowCameraSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {

    for (auto e : _entities)
    {
      if (!ecs.hasComponent<FollowCamera>(e))
        continue;

      auto& camTr  = ecs.getComponent<Transform>(e);
      auto& camera = ecs.getComponent<FollowCamera>(e);

      Vector2D screenCenter{ provider.camera.getViewportWidth() * 0.5f,
                             provider.camera.getViewportHeight() * 0.5f };

      if (!camera.isActive)
        continue;

      if (camera.target != INVALID_ENTITY && ecs.hasComponent<Transform>(camera.target))
      {
        Vector2D targetPos = ecs.getComponent<Transform>(camera.target).position;
        camTr.position     = targetPos;

        provider.camera.setPosition(camTr.position - screenCenter);
      }
    }
  }
};

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

      Vector2D screenCenter{ provider.vw * 0.5f, provider.vh * 0.5f };

      if (!camera.isActive)
        continue;

      if (camera.target != INVALID_ENTITY && ecs.hasComponent<Transform>(camera.target))
      {
        Vector2D targetPos = ecs.getComponent<Transform>(camera.target).position;
        camTr.position     = targetPos;

        provider.setCameraOffset(camTr.position - screenCenter);
      }
    }
  }
};

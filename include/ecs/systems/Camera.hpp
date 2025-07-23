#pragma once

#include "CameraComponents.hpp"
#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Provider.hpp"

class FollowCameraSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {
    for (auto e : _entities)
    {
      auto& camTr  = ecs.getComponent<Transform>(e);
      auto& camera = ecs.getComponent<FollowCamera>(e);

      if (!camera.isActive)
        continue;

      float    zoom = provider.camera.getZoom();
      Vector2D screenCenter{ provider.camera.getViewportWidth() * 0.5f / zoom,
                             provider.camera.getViewportHeight() * 0.5f / zoom };

      provider.camera.setPosition(camTr.position - screenCenter);
    }
  }
};

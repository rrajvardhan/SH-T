#pragma once

#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "RenderableComponents.hpp"
#include "World.hpp"

class RenderSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {
    for (auto entity : _entities)
    {
      auto& tr   = ecs.getComponent<Transform>(entity);
      auto& rend = ecs.getComponent<Renderable>(entity);

      Vector2D screenCenter = tr.position - provider.getCameraOffset();
      Vector2D topLeft      = screenCenter - rend.size * 0.5f;

      switch (rend.shape)
      {
      case Renderable::RECT:
        provider.service.graphics->drawFilledRect(static_cast<int>(topLeft.x),
                                                  static_cast<int>(topLeft.y),
                                                  static_cast<int>(rend.size.x),
                                                  static_cast<int>(rend.size.y),
                                                  rend.color);
        break;
      case Renderable::CIRCLE:
        provider.service.graphics->drawFilledCircle(static_cast<int>(screenCenter.x),
                                                    static_cast<int>(screenCenter.y),
                                                    static_cast<int>(rend.size.x * 0.5f),
                                                    rend.color);
        break;
      }
    }
  }
};

#pragma once

#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Provider.hpp"
#include "RenderableComponents.hpp"

class RenderSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {
    for (auto entity : _entities)
    {
      auto& tr   = ecs.getComponent<Transform>(entity);
      auto& rend = ecs.getComponent<Renderable>(entity);

      float    zoom         = provider.camera.getZoom();
      Vector2D screenCenter = (tr.position - provider.camera.getOffset()) * zoom;
      Vector2D topLeft      = screenCenter - rend.size * 0.5f * zoom;

      switch (rend.shape)
      {
      case Renderable::RECT:
        provider.service.graphics->drawFilledRect(static_cast<int>(topLeft.x),
                                                  static_cast<int>(topLeft.y),
                                                  static_cast<int>(rend.size.x * zoom),
                                                  static_cast<int>(rend.size.y * zoom),
                                                  rend.color);
        break;
      case Renderable::CIRCLE:
        provider.service.graphics->drawFilledCircle(static_cast<int>(screenCenter.x),
                                                    static_cast<int>(screenCenter.y),
                                                    static_cast<int>(rend.size.x * zoom),
                                                    rend.color);
        break;
      }
    }
  }
};

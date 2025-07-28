#pragma once

#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Provider.hpp"
#include "RenderableComponents.hpp"
#include "TextComponents.hpp"

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

      if (ecs.hasComponent<TextComponent>(entity))
      {
        auto& text = ecs.getComponent<TextComponent>(entity);
        if (!text.visible)
          continue;

        float    zoom         = provider.camera.getZoom();
        Vector2D screenCenter = (tr.position - provider.camera.getOffset()) * zoom;
        Vector2D offsetPos    = screenCenter + text.offset * zoom;

        SDL_Rect destRect;
        destRect.x = static_cast<int>(offsetPos.x);
        destRect.y = static_cast<int>(offsetPos.y);

        TTF_Font* font = provider.service.font->getFont(text.fontId);
        if (!font)
          continue;

        int textW, textH;
        if (TTF_SizeText(font, text.text.c_str(), &textW, &textH) != 0)
        {
          LOG_ERROR("[RenderSystem] Failed to measure text: ", TTF_GetError());
          continue;
        }

        destRect.w = static_cast<int>(textW * zoom);
        destRect.h = static_cast<int>(textH * zoom);

        provider.service.font->drawText(text.fontId, text.text, text.color, destRect);
      }
    }
  }
};

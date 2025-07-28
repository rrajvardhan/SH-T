#pragma once

#include "FontManager.hpp"
#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Provider.hpp"
#include "TextComponents.hpp"

class UISystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider& provider)
  {

    std::vector<Entity> sortedEntities(_entities.begin(), _entities.end());

    std::sort(sortedEntities.begin(),
              sortedEntities.end(),
              [&](Entity a, Entity b)
              {
                return ecs.getComponent<TextComponent>(a).layer
                       < ecs.getComponent<TextComponent>(b).layer;
              });

    for (auto entity : sortedEntities)
    {
      if (!ecs.hasComponent<TextComponent>(entity) || !ecs.hasComponent<Transform>(entity))
        continue;

      auto& text = ecs.getComponent<TextComponent>(entity);
      if (!text.visible)
        continue;

      auto& tr   = ecs.getComponent<Transform>(entity);
      float zoom = provider.camera.getZoom();

      Vector2D worldPos  = (tr.position - provider.camera.getOffset()) * zoom;
      Vector2D screenPos = worldPos + text.offset * zoom;

      TTF_Font* font = provider.service.font->getFont(text.fontId);
      if (!font)
        continue;

      int textW, textH;
      if (TTF_SizeText(font, text.text.c_str(), &textW, &textH) != 0)
        continue;

      SDL_Rect destRect;
      if (text.size.x > 0 && text.size.y > 0)
      {
        destRect.w = static_cast<int>(text.size.x * zoom);
        destRect.h = static_cast<int>(text.size.y * zoom);
      }
      else
      {
        destRect.w = static_cast<int>(textW * zoom);
        destRect.h = static_cast<int>(textH * zoom);
      }
      destRect.y = static_cast<int>(screenPos.y);
      destRect.x = static_cast<int>(screenPos.x);

      provider.service.font->drawText(text.fontId, text.text, text.color, destRect);
    }
  }
};

#pragma once

#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "Provider.hpp"
#include "SpriteComponents.hpp"
#include "Vector2D.hpp"
#include <algorithm>

class SpriteSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {

    struct RenderEntry
    {
      Entity entity;
      int    layer;
    };

    std::vector<RenderEntry> sorted;

    for (auto entity : _entities)
    {
      const auto& sprite = ecs.getComponent<Sprite>(entity);
      sorted.push_back({ entity, sprite.layer });
    }

    std::sort(sorted.begin(),
              sorted.end(),
              [](const RenderEntry& a, const RenderEntry& b) { return a.layer < b.layer; });

    for (const auto entry : sorted)
    {
      auto& sprite    = ecs.getComponent<Sprite>(entry.entity);
      auto& transform = ecs.getComponent<Transform>(entry.entity);

      if (sprite.textureId.empty())
        continue;

      float zoom = provider.camera.getZoom();

      int scaledW = (int) (sprite.srcRect.w * sprite.scale * zoom);
      int scaledH = (int) (sprite.srcRect.h * sprite.scale * zoom);

      Vector2D renderPos
          = (transform.position + sprite.offset - provider.camera.getOffset()) * zoom;

      SDL_Rect dest;
      dest.w = scaledW;
      dest.h = scaledH;
      dest.x = (int) (renderPos.x - scaledW / 2.0f);
      dest.y = (int) (renderPos.y - scaledH / 2.0f);

      auto tex = provider.service.texture->getTexture(sprite.textureId);

      provider.service.graphics->setTextureBlendMode(tex, sprite.blendMode);
      provider.service.texture->drawTexture(
          tex, &sprite.srcRect, &dest, transform.rotation, nullptr, sprite.flip);
    }
  }
};

#pragma once

#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "SpriteComponents.hpp"
#include "Vector2D.hpp"
#include "World.hpp"

class SpriteRenderSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {
    for (auto entity : _entities)
    {
      auto& sprite    = ecs.getComponent<Sprite>(entity);
      auto& transform = ecs.getComponent<Transform>(entity);

      int scaledW = (int) (sprite.srcRect.w * sprite.scale);
      int scaledH = (int) (sprite.srcRect.h * sprite.scale);

      SDL_Rect dest;
      dest.w = scaledW;
      dest.h = scaledH;

      Vector2D renderPos = transform.position + sprite.offset;
      dest.x             = (int) (renderPos.x - provider.getCameraOffset().x - scaledW / 2.0f);
      dest.y             = (int) (renderPos.y - provider.getCameraOffset().y - scaledH / 2.0f);

      provider.service.texture->drawTexture(sprite.texture, &sprite.srcRect, &dest);
    }
  }
};

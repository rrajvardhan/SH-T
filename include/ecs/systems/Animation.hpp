#pragma once

#include "Overseer.hpp"
#include "Provider.hpp"
#include "SpriteComponents.hpp"

class AnimationSystem : public System
{
public:
  void update(Overseer& ecs, WorldProvider provider)
  {
    for (auto entity : _entities)
    {
      float dt = provider.service.timer->getDeltaTime();

      auto& animator = ecs.getComponent<SpriteAnimator>(entity);
      auto& sprite   = ecs.getComponent<Sprite>(entity);

      const auto& anim = animator.animations[animator.currentAnim];
      if (animator.animations.empty())
        continue;
      if (anim.frames.empty())
        continue;

      animator.timer += dt * 1000.0f;
      if (animator.timer >= anim.speed)
      {
        animator.timer        = 0.0f;
        animator.currentFrame = (animator.currentFrame + 1) % anim.frames.size();
      }

      const auto& frame = anim.frames[animator.currentFrame];

      if (!anim.textureId.empty())
        sprite.textureId = anim.textureId;

      sprite.srcRect = frame.rect;
      sprite.offset  = frame.offset;
    }
  }
};

inline void
playAnimation(SpriteAnimator& animator, const std::string& textureId)
{
  if (animator.currentAnim != textureId)
  {
    animator.currentAnim  = textureId;
    animator.currentFrame = 0;
    animator.timer        = 0.0f;
  }
}

#pragma once
#include "Overseer.hpp"
#include "SpriteComponents.hpp"

class SpriteAnimationSystem : public System
{
public:
  void update(Overseer& ecs, float dt)
  {
    for (auto entity : _entities)
    {
      auto& animator = ecs.getComponent<SpriteAnimator>(entity);
      auto& sprite   = ecs.getComponent<Sprite>(entity);

      const auto& anim = animator.animations[animator.currentAnim];
      if (anim.frames.empty())
        continue;

      animator.timer += dt * 1000.0f; // convert seconds to milliseconds
      if (animator.timer >= anim.speed)
      {
        animator.timer        = 0.0f;
        animator.currentFrame = (animator.currentFrame + 1) % anim.frames.size();
      }

      sprite.srcRect = anim.frames[animator.currentFrame].rect;
      sprite.offset  = anim.frames[animator.currentFrame].offset;
    }
  }
};

inline void
playAnimation(SpriteAnimator& animator, const std::string& name)
{
  if (animator.currentAnim != name)
  {
    animator.currentAnim  = name;
    animator.currentFrame = 0;
    animator.timer        = 0.0f;
  }
}

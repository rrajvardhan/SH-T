#pragma once

#include "CollisionComponents.hpp"
#include "Overseer.hpp"
#include "PhysicsComponents.hpp"
#include "System.hpp"
#include "Types.hpp"

struct AABB
{
  Vector2D min;
  Vector2D max;
};

inline AABB
getAABB(const Transform& transform, const Collider& collider)
{
  Vector2D halfSize = collider.size * 0.5f;
  Vector2D center   = transform.position + collider.offset;
  return {
    center - halfSize, // min
    center + halfSize  // max
  };
}

inline bool
checkAABBOverlap(const AABB& a, const AABB& b)
{
  return !(a.max.x < b.min.x || a.min.x > b.max.x || a.max.y < b.min.y || a.min.y > b.max.y);
}

class CollisionSystem : public System
{
public:
  void update(Overseer& ecs)
  {
    for (auto it1 = _entities.begin(); it1 != _entities.end(); ++it1)
    {
      for (auto it2 = std::next(it1); it2 != _entities.end(); ++it2)
      {
        Entity a = *it1;
        Entity b = *it2;

        auto& tA = ecs.getComponent<Transform>(a);
        auto& cA = ecs.getComponent<Collider>(a);
        auto& tB = ecs.getComponent<Transform>(b);
        auto& cB = ecs.getComponent<Collider>(b);

        AABB aBox = getAABB(tA, cA);
        AABB bBox = getAABB(tB, cB);

        if (!checkAABBOverlap(aBox, bBox))
          continue;

        bool aStatic = cA.isStatic;
        bool bStatic = cB.isStatic;

        if (aStatic && bStatic)
          continue;

        if (!aStatic && !bStatic)
        {
          resolveDynamicVsDynamic(ecs, a, b);
        }
        else
        {
          resolveDynamicVsStatic(ecs, a, b);
        }
      }
    }
  }

private:
  void resolveDynamicVsStatic(Overseer& ecs, Entity a, Entity b)
  {
    Entity dyn  = ecs.getComponent<Collider>(a).isStatic ? b : a;
    Entity stat = ecs.getComponent<Collider>(a).isStatic ? a : b;

    auto& tD  = ecs.getComponent<Transform>(dyn);
    auto& cD  = ecs.getComponent<Collider>(dyn);
    auto& rbD = ecs.getComponent<RigidBody>(dyn);

    auto& tS = ecs.getComponent<Transform>(stat);
    auto& cS = ecs.getComponent<Collider>(stat);

    Vector2D dynCenter  = tD.position + cD.offset;
    Vector2D statCenter = tS.position + cS.offset;
    Vector2D delta      = dynCenter - statCenter;
    Vector2D totalHalf  = (cD.size + cS.size) * 0.5f;

    float overlapX = totalHalf.x - std::abs(delta.x);
    float overlapY = totalHalf.y - std::abs(delta.y);

    if (overlapX < overlapY)
    {
      float dir = delta.x < 0 ? -1.f : 1.f;
      tD.position.x += overlapX * dir;
      rbD.velocity.x     = 0;
      rbD.acceleration.x = 0;
    }
    else
    {
      float dir = delta.y < 0 ? -1.f : 1.f;
      tD.position.y += overlapY * dir;
      rbD.velocity.y = 0;
      if (dir < 0)
        rbD.acceleration.y = 0;
    }
  }

  void resolveDynamicVsDynamic(Overseer& ecs, Entity a, Entity b)
  {
    auto& tA  = ecs.getComponent<Transform>(a);
    auto& cA  = ecs.getComponent<Collider>(a);
    auto& rbA = ecs.getComponent<RigidBody>(a);

    auto& tB  = ecs.getComponent<Transform>(b);
    auto& cB  = ecs.getComponent<Collider>(b);
    auto& rbB = ecs.getComponent<RigidBody>(b);

    Vector2D delta = (tA.position + cA.offset) - (tB.position + cB.offset);
    Vector2D half  = (cA.size + cB.size) * 0.5f;

    float overlapX = half.x - std::abs(delta.x);
    float overlapY = half.y - std::abs(delta.y);

    if (overlapX < 0 || overlapY < 0)
      return;

    Vector2D mtv;
    if (overlapX < overlapY)
    {
      float dir = delta.x < 0 ? -1.f : 1.f;
      mtv       = { overlapX * dir, 0.f };
    }
    else
    {
      float dir = delta.y < 0 ? -1.f : 1.f;
      mtv       = { 0.f, overlapY * dir };
    }

    float totalMass = rbA.mass + rbB.mass;
    float aRatio    = rbB.mass / totalMass;
    float bRatio    = rbA.mass / totalMass;

    tA.position += mtv * aRatio;
    tB.position -= mtv * bRatio;

    Vector2D normal = (mtv.x != 0) ? Vector2D{ mtv.x < 0 ? -1.f : 1.f, 0.f }
                                   : Vector2D{ 0.f, mtv.y < 0 ? -1.f : 1.f };

    Vector2D relativeVel = rbA.velocity - rbB.velocity;
    float    velAlongN   = relativeVel.x * normal.x + relativeVel.y * normal.y;

    if (velAlongN > 0)
      return;

    float restitution = 0.2f;

    float impulseMag = -(1 + restitution) * velAlongN;
    impulseMag /= (1.f / rbA.mass + 1.f / rbB.mass);

    Vector2D impulse = impulseMag * normal;
    rbA.velocity += impulse * (1.f / rbA.mass);
    rbB.velocity -= impulse * (1.f / rbB.mass);
  }
};

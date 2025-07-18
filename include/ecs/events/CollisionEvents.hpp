#pragma once

#include "Event.hpp"
#include "Types.hpp"

struct CollisionEvent : public Event
{
  Entity a;
  Entity b;

  CollisionEvent(Entity a, Entity b) : a(a), b(b) {}

  std::string to_string() const override { return "CollisionEvent"; }
};

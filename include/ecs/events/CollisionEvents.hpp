#pragma once

#include "Event.hpp"
#include "Types.hpp"

struct CollisionEvent : public Event
{
  Entity a;
  Entity b;

  CollisionEvent(Entity a_, Entity b_) : a(a_), b(b_) {}
};

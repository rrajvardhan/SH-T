#pragma once

#include "Vector2D.hpp"

struct Transform
{
  Vector2D position = { 0, 0 };
  float    rotation = 0.0f;
  Vector2D scale    = { 1, 1 };
};

struct RigidBody
{
  Vector2D velocity     = { 0, 0 };
  Vector2D acceleration = { 0, 0 };
  float    mass         = 1.0f;
};

struct Force
{
  Vector2D vector = { 0, 0 };
};

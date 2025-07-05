#pragma once

#include <cmath>

constexpr float PI         = 3.14159265f;
constexpr float DEG_TO_RAD = PI / 180.0f;

struct Vector2D
{
  float x = 0.0f;
  float y = 0.0f;

  constexpr Vector2D() = default;
  constexpr Vector2D(float _x, float _y) : x(_x), y(_y) {}

  float magnitudeSqr() const { return x * x + y * y; }

  float magnitude() const { return std::sqrt(magnitudeSqr()); }

  float dot(const Vector2D& other) const { return x * other.x + y * other.y; }

  Vector2D normalized() const
  {
    float mag = magnitude();
    return (mag != 0.0f) ? Vector2D(x / mag, y / mag) : Vector2D(0.0f, 0.0f);
  }

  Vector2D& operator+=(const Vector2D& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  Vector2D& operator-=(const Vector2D& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
  }

  Vector2D& operator*=(float scalar)
  {
    x *= scalar;
    y *= scalar;
    return *this;
  }
};

// -------- Arithmetic Operators -------- //

inline Vector2D
operator+(const Vector2D& lhs, const Vector2D& rhs)
{
  return Vector2D(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline Vector2D
operator-(const Vector2D& lhs, const Vector2D& rhs)
{
  return Vector2D(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline Vector2D
operator*(const Vector2D& vec, float scalar)
{
  return Vector2D(vec.x * scalar, vec.y * scalar);
}

inline Vector2D
operator*(float scalar, const Vector2D& vec)
{
  return vec * scalar;
}

inline Vector2D
operator/(const Vector2D& vec, float scalar)
{
  return (scalar != 0.0f) ? Vector2D(vec.x / scalar, vec.y / scalar) : Vector2D(0.0f, 0.0f);
}

// -------- Math Utilities -------- //

inline Vector2D
rotateVector(const Vector2D& vec, float angleDeg)
{
  float rad  = angleDeg * DEG_TO_RAD;
  float cosA = std::cos(rad);
  float sinA = std::sin(rad);
  return Vector2D(vec.x * cosA - vec.y * sinA, vec.x * sinA + vec.y * cosA);
}

// -------- Constants -------- //

constexpr Vector2D VEC2_ZERO  = { 0.0f, 0.0f };
constexpr Vector2D VEC2_ONE   = { 1.0f, 1.0f };
constexpr Vector2D VEC2_UP    = { 0.0f, 1.0f };
constexpr Vector2D VEC2_DOWN  = { 0.0f, -1.0f };
constexpr Vector2D VEC2_LEFT  = { -1.0f, 0.0f };
constexpr Vector2D VEC2_RIGHT = { 1.0f, 0.0f };

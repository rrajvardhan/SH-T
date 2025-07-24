#pragma once
#include "Vector2D.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <vector>

struct Sprite
{
  std::string textureId;
  SDL_Rect    srcRect{ 0, 0, 0, 0 };
  float       scale  = 1.0f;
  std::string flip   = "none";
  Vector2D    offset = { 0.0f, 0.0f };
  int         layer  = 0;

  std::string blendMode = "blend";
};

struct AnimationFrame
{
  SDL_Rect rect;
  Vector2D offset;

  AnimationFrame(SDL_Rect r, Vector2D o = { 0, 0 }) : rect(r), offset(o) {}
};

struct Animation
{
  std::vector<AnimationFrame> frames;
  int                         speed = 100;
  std::string                 textureId;

  Animation() = default;
  Animation(std::vector<AnimationFrame> af, int s, std::string name)
      : frames(af), speed(s), textureId(name)
  {
  }
};

struct SpriteAnimator
{
  std::string currentAnim;

  std::unordered_map<std::string, Animation> animations;

  int   currentFrame = 0;
  float timer        = 0.0f;
};

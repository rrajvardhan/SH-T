#pragma once
#include "Vector2D.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <vector>

struct Sprite
{
  SDL_Texture*     texture = nullptr;
  SDL_Rect         srcRect{ 0, 0, 0, 0 };
  float            scale  = 1.0f;
  SDL_RendererFlip flip   = SDL_FLIP_NONE;
  Vector2D         offset = { 0.0f, 0.0f }; // new
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
  int                         speed = 100; // ms per frame

  Animation() = default;
  Animation(std::vector<AnimationFrame> af, int s) : frames(af), speed(s) {}
};

struct SpriteAnimator
{
  std::string                                currentAnim;
  std::unordered_map<std::string, Animation> animations;

  int   currentFrame = 0;
  float timer        = 0.0f;
};

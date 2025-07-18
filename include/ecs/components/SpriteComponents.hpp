#pragma once
#include "Vector2D.hpp"
#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>
#include <vector>

struct Sprite
{
  std::string      name;
  SDL_Rect         srcRect{ 0, 0, 0, 0 };
  float            scale  = 1.0f;
  SDL_RendererFlip flip   = SDL_FLIP_NONE;
  Vector2D         offset = { 0.0f, 0.0f };
  int              layer  = 0;

  SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
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

#include "Timer.hpp"
#include <SDL2/SDL_timer.h>

Timer::Timer() {
  _lastTicks = SDL_GetTicks();
  _timeScale = 1.0f;
}

Timer::~Timer() {}

void Timer::reset() {
  _startTicks = SDL_GetTicks();
  _elapsedTicks = 0;
  _deltaTime = 0.0f;
}

void Timer::update() {
  Uint32 currentTicks = SDL_GetTicks();
  Uint32 frameTicks = currentTicks - _lastTicks;

  _deltaTime = (frameTicks * 0.001f) * _timeScale;
  _lastTicks = currentTicks;
}

float Timer::getDeltaTime() const { return _deltaTime; }

void Timer::setTimeScale(float tScale) { _timeScale = tScale; }

float Timer::getTimeScale() const { return _timeScale; }

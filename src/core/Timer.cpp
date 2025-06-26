#include "Timer.hpp"
#include <SDL2/SDL_timer.h>

Timer::Timer() {
  reset();
  _timeScale = 1.0f;
}

Timer::~Timer() {}

void Timer::reset() {
  _startTicks = SDL_GetTicks();
  _elapsedTicks = 0;
  // _deltaTime = 0.0f;
}

void Timer::update() {
  _elapsedTicks = SDL_GetTicks() - _startTicks;
  _deltaTime = (_elapsedTicks * 0.001f) * _timeScale;
}

float Timer::getDeltaTime() const { return _deltaTime; }

void Timer::setTimeScale(float tScale) { _timeScale = tScale; }

float Timer::getTimeScale() const { return _timeScale; }

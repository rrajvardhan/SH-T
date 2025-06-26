#pragma once

class Timer {
public:
  Timer();
  ~Timer();

  void reset();
  void update();

  float getDeltaTime() const;
  float getTimeScale() const;
  void setTimeScale(float tScale);

private:
  unsigned int _startTicks = 0;
  unsigned int _elapsedTicks = 0;

  float _deltaTime = 0.0f;
  float _timeScale = 1.0f;
};

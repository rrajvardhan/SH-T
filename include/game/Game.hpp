#pragma once

#include "EngineContext.hpp"

class Game {
public:
  Game(EngineContext &ctx);
  ~Game();

  bool init();

  void update();
  void render();

  bool isRunning() const;

private:
  bool _running;

  EngineContext &_ctx;
};

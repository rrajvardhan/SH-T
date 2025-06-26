#include "Game.hpp"
#include "Log.hpp"

Game::Game(EngineContext &ctx) : _running(true), _ctx(ctx) {}

Game::~Game() {}

bool Game::init() {

  LOG_INFO("[Game] Game initialized.");

  return true;
}

void Game::update() {
  if (_ctx.input->keyPressed(SDL_SCANCODE_ESCAPE)) {
    _running = false;
  }
}

void Game::render() {}

bool Game::isRunning() const { return _running; }

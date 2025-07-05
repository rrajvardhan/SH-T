#include "Editor.hpp"
#include "Services.hpp"
#include "World.hpp"

class Application
{
public:
  bool init();
  bool init(EngineConfig& cfg);
  void run();

private:
  void handleResizeEvent(int w, int h);

  void pollEvents();
  void capFPS();
  void processInput();
  void update();
  void render();

  int _width  = 1600;
  int _height = 900;

  Services       _engine;
  ServiceContext _ctx;
  EngineConfig   _cfg;

  SDL_Event _event;
  bool      _running = false;
  bool      _quit    = false;

  World*  _world  = nullptr;
  Editor* _editor = nullptr;
};

#include "Application.hpp"
#include "Log.hpp"
#include "ServiceContext.hpp"
#include "World.hpp"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

bool
Application::init()
{
  return init(_cfg);
}

bool
Application::init(EngineConfig& cfg)
{

  if (!_engine.init(cfg))
  {
    LOG_ERROR("[Application] Services failed to initialize.");
    return false;
  }
  _cfg = cfg;
  _ctx = _engine.getContext();

  _world = new World(_ctx);
  if (!_world->init())
  {
    LOG_ERROR("World failed to initialize.");
    return false;
  }

  _world->setViewport(_ctx.graphics->getScreenWidth(), _ctx.graphics->getScreenHeight());

  _editor = new Editor(_world, _ctx);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void) io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  io.FontGlobalScale = 1.5f;
  io.Fonts->Clear();
  ImFont* customFont = io.Fonts->AddFontFromFileTTF("assets/default.ttf", 18.0f);
  if (!customFont)
  {
    LOG_ERROR("[ImGui] Failed to load JetBrains Mono font!");
  }

  ImGui::StyleColorsClassic();

  ImGuiStyle& style   = ImGui::GetStyle();
  style.WindowPadding = ImVec2(0, 0);
  style.ScrollbarSize = 0;

  ImGui_ImplSDLRenderer2_CreateDeviceObjects();
  ImGui_ImplSDL2_InitForSDLRenderer(_ctx.graphics->getWindow(), _ctx.graphics->getRenderer());
  ImGui_ImplSDLRenderer2_Init(_ctx.graphics->getRenderer());

  _running = true;
  LOG_SUCCESS("[Application] Initialized.");
  return true;
}

void
Application::run()
{
  while (_running && !_quit)
  {

    _ctx.timer->update();

    pollEvents();
    _ctx.input->update();
    _ctx.graphics->clear();

    processInput();
    update();
    render();

    _ctx.graphics->present();
    _ctx.input->updatePrev();
    _ctx.timer->reset();

    capFPS();
  }
}

void
Application::pollEvents()
{
  while (SDL_PollEvent(&_event))
  {
    ImGui_ImplSDL2_ProcessEvent(&_event);
    switch (_event.type)
    {
    case SDL_QUIT:
      _quit = true;
      break;

    case SDL_WINDOWEVENT:
      if (_event.window.event == SDL_WINDOWEVENT_RESIZED
          || _event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
      {
        _width  = _event.window.data1;
        _height = _event.window.data2;

        _cfg.renderer.width  = _width;
        _cfg.renderer.height = _height;

        _ctx.graphics->onResize(_width, _height);
        _world->setViewport(_width, _height);
      }
      break;
    }
  }
}

void
Application::capFPS()
{

  float dt        = _ctx.timer->getDeltaTime();
  float frameTime = 1.0f / _cfg.targetFPS;

  if (_cfg.targetFPS > 0 && dt < frameTime)
  {
    float delay = (frameTime - dt) * 1000.0f;
    SDL_Delay((Uint32) delay);
    dt = frameTime;
  }
}

void
Application::processInput()
{
  if (_ctx.input->keyDown(SDL_SCANCODE_ESCAPE))
  {
    _running = false;
  }
}

void
Application::update()
{
  _editor->update();

  if (!_editor->isActive())
    _world->update();
}

void
Application::render()
{
  ImGui_ImplSDL2_NewFrame();
  ImGui_ImplSDLRenderer2_NewFrame();
  ImGui::NewFrame();

  _editor->render();

  ImGui::Render();
  ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), _ctx.graphics->getRenderer());
}

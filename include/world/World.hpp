#pragma once

#include "Camera2D.hpp"
#include "GlobalScriptSystem.hpp"
#include "Overseer.hpp"
#include "Provider.hpp"
#include "ServiceContext.hpp"
#include "SystemEntry.hpp"
#include <algorithm>
#include <vector>

class World
{
public:
  World(ServiceContext& ctx);
  ~World();

  bool init();
  void update();
  void render();

  WorldProvider&            getProvider() { return _provider; }
  Overseer&                 getECS() { return ecs; }
  Camera2D&                 getCamera() { return _camera; }
  std::vector<SystemEntry>& getSystems() { return _systems; }

  void resortSystems()
  {
    std::sort(_systems.begin(),
              _systems.end(),
              [](const SystemEntry& a, const SystemEntry& b) { return a.order < b.order; });
  }

private:
  Overseer                 ecs;
  Camera2D                 _camera;
  ServiceContext&          _ctx;
  WorldProvider            _provider;
  sol::state               _lua;
  GlobalScriptSystem*      _globalScript;
  std::vector<SystemEntry> _systems;

  template <typename T>
  std::shared_ptr<T>
  registerSystem(const std::string& name, int order, std::string phase = "update")
  {
    auto system = ecs.registerSystem<T>();

    SystemEntry entry = { .name    = name,
                          .phase   = phase,
                          .order   = order,
                          .enabled = true,
                          .func    = [system](Overseer& ecs, const WorldProvider& provider)
                          { system->update(ecs, provider); } };

    _systems.push_back(std::move(entry));
    std::sort(_systems.begin(), _systems.end(), [](auto& a, auto& b) { return a.order < b.order; });

    return system;
  }
};

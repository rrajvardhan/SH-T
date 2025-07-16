#pragma once

#include "Overseer.hpp"
#include "sol/state.hpp"
#include <sol/sol.hpp>

class GlobalScriptSystem
{
public:
  GlobalScriptSystem();
  void update(float dt);

  void bind(Overseer& ecs);
  void loadScript(const std::string& path);

  void reload();

  sol::state& getLuaState() { return _lua; }

private:
  sol::state    _lua;
  sol::function _updatefunc;
  std::string   _scriptPath;

  Overseer* _ecs = nullptr;
};

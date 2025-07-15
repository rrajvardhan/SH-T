#pragma once

#include "Overseer.hpp"
#include <sol/sol.hpp>

class GlobalScriptSystem
{
public:
  GlobalScriptSystem();
  void update(float dt);

  void bind(Overseer& ecs);
  void loadScript(const std::string& path);

private:
  sol::state    _lua;
  sol::function _updatefunc;
  std::string   _scriptPath;

  Overseer* _ecs = nullptr;
};

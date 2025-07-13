#pragma once

#include <sol/sol.hpp>

class GlobalScriptSystem
{
public:
  GlobalScriptSystem(sol::state& lua, const std::string& scriptpath);
  void update(float dt);

private:
  sol::state&   _lua;
  sol::function _updatefunc;
  std::string   _scriptPath;
};

#include "GlobalScriptSystem.hpp"
#include "Log.hpp"
#include "LuaBindings.hpp"
#include "Overseer.hpp"

GlobalScriptSystem::GlobalScriptSystem()
{
  _lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
}

void
GlobalScriptSystem::bind(Overseer& ecs)
{
  _ecs = &ecs;
}

void
GlobalScriptSystem::loadScript(const std::string& path)
{
  _scriptPath             = path;
  sol::load_result script = _lua.load_file(path);
  if (!script.valid())
  {
    sol::error err = script;
    LOG_ERROR("[GlobalScriptSystem] Failed to load '", path, "': ", err.what());
    return;
  }

  sol::protected_function_result result = script();
  if (!result.valid())
  {
    sol::error err = result;
    LOG_ERROR("[GlobalScriptSystem] Runtime error in script '", path, "': ", err.what());
    return;
  }

  _updatefunc = _lua["update"];
}

void
GlobalScriptSystem::update(float dt)
{
  if (_updatefunc.valid())
  {
    sol::protected_function_result result = _updatefunc(dt);
    if (!result.valid())
    {
      sol::error err = result;
      LOG_ERROR("[GlobalScriptSystem] update() error: ", err.what());
    }
  }
}

void
GlobalScriptSystem::reload()
{
  if (!_scriptPath.empty())
    loadScript(_scriptPath);
}

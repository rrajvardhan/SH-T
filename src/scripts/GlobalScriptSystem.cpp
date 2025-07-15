#include "GlobalScriptSystem.hpp"
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

  LuaBindings::bindVector2D(_lua);
  LuaBindings::bindTransform(_lua, *_ecs);
  LuaBindings::bindFollowCamera(_lua, *_ecs);
}

void
GlobalScriptSystem::loadScript(const std::string& path)
{
  sol::load_result script = _lua.load_file(path);
  if (!script.valid())
  {
    sol::error err = script;
    LOG_ERROR("[GlobalScriptSystem] Failed to load '", path, "': ", err.what());
    return;
  }

  script();
  _updatefunc = _lua["update"];
}

void
GlobalScriptSystem::update(float dt)
{
  if (_updatefunc.valid())
    _updatefunc(dt);
}

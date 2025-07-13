#include "GlobalScriptSystem.hpp"
#include "Log.hpp"

GlobalScriptSystem::GlobalScriptSystem(sol::state& lua, const std::string& scriptpath)
    : _lua(lua), _scriptPath(scriptpath)
{
  sol::load_result script = _lua.load_file(_scriptPath);
  if (!script.valid())
  {
    sol::error err = script;
    LOG_ERROR("[GlobalScriptSystem] ", err.what());
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

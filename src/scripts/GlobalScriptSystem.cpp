#include "CollisionEvents.hpp"
#include "GlobalScriptSystem.hpp"
#include "Log.hpp"
#include "LuaBindings.hpp"
#include "Overseer.hpp"

GlobalScriptSystem::GlobalScriptSystem()
{
  _lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string);
  _lua.script(R"(package.path = "scripts/?.lua;" .. package.path)");
}

void
GlobalScriptSystem::bind(Overseer& ecs)
{
  _ecs = &ecs;

  _lua.set_function("subscribe_event",
                    [this](const std::string& eventName, sol::function cb)
                    { subscribe_lua(eventName, cb); });

  _lua.set_function("publish_event",
                    [this](const std::string& eventName, sol::table payload)
                    { publish_lua(eventName, payload); });
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

void
GlobalScriptSystem::subscribe_lua(const std::string& eventName, sol::function callback)
{
  _luaSubscribers[eventName].push_back(callback);
}

void
GlobalScriptSystem::publish_lua(const std::string& eventName, const sol::table& payload)
{
  auto it = _luaSubscribers.find(eventName);
  if (it == _luaSubscribers.end())
    return;

  for (auto& cb : it->second)
    cb(payload);
}

void
GlobalScriptSystem::onAnyEvent(CollisionEvent* e)
{
  sol::table payload = _lua.create_table();

  payload["entity_a"] = e->a;
  payload["entity_b"] = e->b;

  auto it = _luaSubscribers.find("CollisionEvent");
  if (it == _luaSubscribers.end())
    return;

  for (auto& cb : it->second)
  {
    sol::protected_function_result result = cb(payload);
    if (!result.valid())
    {
      sol::error err = result;
      LOG_ERROR("[Lua] CollisionEvent handler error: ", err.what());
    }
  }
}

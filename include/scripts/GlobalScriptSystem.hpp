#pragma once

#include "CollisionEvents.hpp"
#include "Overseer.hpp"
#include "sol/state.hpp"
#include <Event.hpp>
#include <map>
#include <sol/sol.hpp>
#include <vector>

class GlobalScriptSystem
{
public:
  GlobalScriptSystem();

  void        update();
  void        bind(Overseer& ecs);
  void        loadScript(const std::string& path);
  void        reload();
  void        reload(std::string& path);
  sol::state& getLuaState() { return _lua; }

  void onCollision(CollisionEvent* e);

private:
  sol::state    _lua;
  sol::function _updatefunc;
  std::string   _scriptPath;
  Overseer*     _ecs = nullptr;

  void subscribe_lua(const std::string& eventName, sol::function callback);
  void publish_lua(const std::string& eventName, const sol::table& payload);

  std::map<std::string, std::vector<sol::function>> _luaSubscribers;
};

#pragma once

#include "AudioManager.hpp"
#include "Camera2D.hpp"
#include "InputManager.hpp"
#include "SceneManager.hpp"
#include "Timer.hpp"
#include "sol/state.hpp"
#include <sol/sol.hpp>

class Overseer;

namespace LuaBindings
{

template <typename T>
void
bindECSComponent(sol::state& lua, Overseer& ecs, const std::string& name)
{
  lua.set_function("get_" + name, [&ecs](Entity e) -> T* { return &ecs.getComponent<T>(e); });
  lua.set_function("add_" + name, [&ecs](Entity e) { ecs.addComponent(e, T{}); });
  lua.set_function("remove_" + name, [&ecs](Entity e) { ecs.removeComponent<T>(e); });
}

void
bindECSCore(sol::state& lua, Overseer& ecs);

void
bindEntityLookup(sol::state& lua, Overseer& ecs);

void
bindKeyConstants(sol::state& lua);

void
bindInput(sol::state& lua, InputManager& input);

void
bindTimer(sol::state& lua, Timer& timer);

void
bindAudio(sol::state& lua, AudioManager& audio);

void
bindCamera2D(sol::state& lua, Camera2D& camera2D);

void
bindSceneManager(sol::state& lua, SceneManager& sceneManager);

void
bindAnimation(sol::state& lua, Overseer& ecs);
}

#pragma once

#include "AudioManager.hpp"
#include "Camera2D.hpp"
#include "Graphics.hpp"
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
  lua.set_function("get_" + name,
                   [&ecs](Entity e) -> T*
                   {
                     if (ecs.hasComponent<T>(e))
                     {
                       return &ecs.getComponent<T>(e);
                     }
                     return nullptr; // nil in Lua
                   });

  lua.set_function("add_" + name,
                   [&ecs](Entity e)
                   {
                     if (!ecs.hasComponent<T>(e))
                     {
                       ecs.addComponent(e, T{});
                     }
                   });

  lua.set_function("remove_" + name,
                   [&ecs](Entity e)
                   {
                     if (ecs.hasComponent<T>(e))
                     {
                       ecs.removeComponent<T>(e);
                     }
                   });

  lua.set_function("has_" + name, [&ecs](Entity e) -> bool { return ecs.hasComponent<T>(e); });
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
bindGraphics(sol::state& lua, Graphics& graphics);

void
bindCamera2D(sol::state& lua, Camera2D& camera2D);

void
bindSceneManager(sol::state& lua, SceneManager& sceneManager);

void
bindAnimation(sol::state& lua, Overseer& ecs);
}

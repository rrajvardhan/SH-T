#pragma once

#include "Camera2D.hpp"
#include "InputManager.hpp"
#include "World.hpp"
#include "sol/state.hpp"
#include <sol/sol.hpp>

class Overseer;

namespace LuaBindings
{
void
bindTransform(sol::state& lua, Overseer& ecs);

void
bindVector2D(sol::state& lua);

void
bindRigigBody(sol::state& lua, Overseer& ecs);

void
bindForce(sol::state& lua, Overseer& ecs);

void
bindFollowCamera(sol::state& lua, Overseer& ecs);

void
bindCollider(sol::state& lua, Overseer& ecs);

void
bindCamera2D(sol::state& lua, Camera2D& camera);

void
bindRigidBody(sol::state& lua, Overseer& ecs);

void
bindKeyConstants(sol::state& lua);

void
bindInput(sol::state& lua, InputManager& input);
}

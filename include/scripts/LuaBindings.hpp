#pragma once

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

}

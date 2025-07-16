#pragma once

#include "ServiceContext.hpp"
#include "Types.hpp"
#include "World.hpp"

namespace Panels
{
void
renderDockspace(World& world);
void
renderGamePanel(bool& active, ServiceContext& ctx, World& world);
void
renderComponentInspector(Entity& selected, World& world);
void
renderControls(bool& active);
void
renderResources(ServiceContext& ctx, World& world);
void
renderEntityPanel(World& world);
}

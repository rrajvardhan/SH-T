#pragma once

#include "ServiceContext.hpp"
#include "World.hpp"
#include <vector>
namespace Panels
{
void
renderDockspace(World& world);
void
renderGamePanel(bool& active, ServiceContext& ctx, World& world);
void
renderEntityList(World& world, Entity& selected, std::vector<Entity>& destroyQueue);
void
renderComponentInspector(Entity& selected, World& world);
void
renderControls(bool& active);
void
renderDebugInfo();
}

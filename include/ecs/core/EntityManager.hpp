#pragma once

#include "Log.hpp"
#include "Types.hpp"
#include <array>
#include <cstdint>
#include <queue>
#include <set>

class EntityManager
{
public:
  EntityManager()
  {
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
    {
      _availableEntities.push(entity);
    }
  }
  Entity createEntity()
  {

    if (_livingEntityCount >= MAX_ENTITIES)
    {
      LOG_DEBUG("[EntityManager] Too many entities exists, cannot create more!!");
    }

    Entity id = _availableEntities.front();
    _availableEntities.pop();
    _livingEntityCount++;
    _livingEntities.insert(id);

    return id;
  }

  void destroyEntity(Entity entity)
  {

    if (entity >= MAX_ENTITIES)
    {
      LOG_DEBUG("[EntityManager] Entity out of range!!");
    }

    _signatures[entity].reset();
    _availableEntities.push(entity);
    _livingEntityCount--;
    _livingEntities.erase(entity);
  }

  void setSignature(Entity entity, Signature signature)
  {

    if (entity >= MAX_ENTITIES)
    {
      LOG_DEBUG("[EntityManager] Entity out of range!!");
    }

    _signatures[entity] = signature;
  }

  Signature getSignature(Entity entity)
  {

    if (entity >= MAX_ENTITIES)
    {
      LOG_DEBUG("[EntityManager] Entity out of range!!");
    }

    return _signatures[entity];
  }

  std::set<Entity>& getLivingEntities() { return _livingEntities; }

private:
  std::queue<Entity>                  _availableEntities{};
  std::array<Signature, MAX_ENTITIES> _signatures{};
  uint32_t                            _livingEntityCount{};

  std::set<Entity> _livingEntities;
};

#pragma once

#include "Log.hpp"
#include "Types.hpp"
#include <array>
#include <queue>

class EntityManager {
public:
  EntityManager() {
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++) {
      _availableEntities.push(entity);
    }
  }
  Entity createEntity() {

    if (_livingEntityCount > MAX_ENTITIES) {
      LOG_DEBUG("[EntityManager] Too many entities exists, cannot create more!!");
    }

    Entity id = _availableEntities.front();
    _availableEntities.pop();
    _livingEntityCount++;

    return id;
  }

  void destroyEntity(Entity entity) {

    if (entity > MAX_ENTITIES) {
      LOG_DEBUG("[EntityManager] Entity out of range!!");
    }

    _signatures[entity].reset();
    _availableEntities.push(entity);
    _livingEntityCount--;
  }

  void setSignature(Entity entity, Signature signature) {

    if (entity > MAX_ENTITIES) {
      LOG_DEBUG("[EntityManager] Entity out of range!!");
    }

    _signatures[entity] = signature;
  }

  Signature getSignature(Entity entity) {

    if (entity > MAX_ENTITIES) {
      LOG_DEBUG("[EntityManager] Entity out of range!!");
    }

    return _signatures[entity];
  }

private:
  std::queue<Entity> _availableEntities{};
  std::array<Signature, MAX_ENTITIES> _signatures{};
  uint32_t _livingEntityCount{};
};

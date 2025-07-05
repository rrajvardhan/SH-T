#pragma once

#include "Log.hpp"
#include "Types.hpp"
#include <unordered_map>

class IComponentArray
{
public:
  virtual ~IComponentArray()                = default;
  virtual void destroyEntity(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray
{
public:
  void insertData(Entity entity, T component)
  {

    if (_entityToIndex.find(entity) != _entityToIndex.end())
    {
      LOG_DEBUG("[ComponentArray] Component added to same entity more than once.");
    }

    size_t newIndex           = _size;
    _entityToIndex[entity]    = newIndex;
    _indexToEntity[newIndex]  = entity;
    _componentArray[newIndex] = component;
    _size++;
  }

  void removeData(Entity entity)
  {

    if (_entityToIndex.find(entity) == _entityToIndex.end())
    {
      LOG_DEBUG("[ComponentArray] Removing non-existent component.");
    }

    size_t removedEntityIndex = _entityToIndex[entity];
    size_t lastElementIndex   = _size - 1;

    _componentArray[removedEntityIndex] = _componentArray[lastElementIndex];

    Entity lastElementEntity           = _indexToEntity[lastElementIndex];
    _entityToIndex[lastElementEntity]  = removedEntityIndex;
    _indexToEntity[removedEntityIndex] = lastElementEntity;

    _entityToIndex.erase(entity);
    _indexToEntity.erase(lastElementIndex);

    _size--;
  }
  void destroyEntity(Entity entity) override
  {

    if (_entityToIndex.find(entity) != _entityToIndex.end())
    {
      removeData(entity);
    }
  }

  T& getData(Entity entity)
  {

    if (_entityToIndex.find(entity) == _entityToIndex.end())
    {
      LOG_DEBUG("[ComponentArray] Retrieving non-existent component.");
    }

    return _componentArray[_entityToIndex[entity]];
  }

  bool has(Entity entity) const { return _entityToIndex.find(entity) != _entityToIndex.end(); }

private:
  std::array<T, MAX_ENTITIES>        _componentArray{};
  std::unordered_map<Entity, size_t> _entityToIndex{};
  std::unordered_map<size_t, Entity> _indexToEntity{};

  size_t _size{};
};

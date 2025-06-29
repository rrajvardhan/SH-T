#pragma once

#include "ComponentArray.hpp"
#include "Types.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>

class ComponentManager {
public:
  template <typename T> void registerComponent() {
    std::type_index typeId = std::type_index(typeid(T));

    if (_componentTypes.find(typeId) != _componentTypes.end()) {
      LOG_DEBUG("[ComponentManager] Registering component type more than once.");
    }

    _componentTypes[typeId] = _nextComponentType;
    _componentArrays[typeId] = std::make_shared<ComponentArray<T>>();

    ++_nextComponentType;
  }

  template <typename T> ComponentType getComponentType() {
    std::type_index typeId = std::type_index(typeid(T));

    if (_componentTypes.find(typeId) == _componentTypes.end()) {
      LOG_DEBUG("[ComponentManager] Component not registered before use.");
    }

    return _componentTypes[typeId];
  }

  template <typename T> void addComponent(Entity entity, T component) {
    getComponentArray<T>()->insertData(entity, component);
  }

  template <typename T> void removeComponent(Entity entity) { getComponentArray<T>()->removeData(entity); }

  template <typename T> T &getComponent(Entity entity) { return getComponentArray<T>()->getData(entity); }

  void destroyEntity(Entity entity) {
    for (auto const &pair : _componentArrays) {
      pair.second->destroyEntity(entity);
    }
  }

private:
  std::unordered_map<std::type_index, ComponentType> _componentTypes{};
  std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> _componentArrays{};
  ComponentType _nextComponentType{};

  template <typename T> std::shared_ptr<ComponentArray<T>> getComponentArray() {
    std::type_index typeId = std::type_index(typeid(T));

    if (_componentTypes.find(typeId) == _componentTypes.end()) {
      LOG_DEBUG("[ComponentManager] Component not registered before use.");
    }

    return std::static_pointer_cast<ComponentArray<T>>(_componentArrays[typeId]);
  }
};

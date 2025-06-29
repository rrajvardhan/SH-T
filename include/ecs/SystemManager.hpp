#pragma once

#include "Log.hpp"
#include "System.hpp"
#include "Types.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>

class SystemManager {
public:
  template <typename T> std::shared_ptr<T> registerSystem() {
    std::type_index typeId = std::type_index(typeid(T));

    if (_systems.find(typeId) != _systems.end()) {
      LOG_DEBUG("[SystemManager] Registering system more than once.");
    }

    auto system = std::make_shared<T>();
    _systems[typeId] = system;
    return system;
  }

  template <typename T> void setSignature(Signature signature) {
    std::type_index typeId = std::type_index(typeid(T));

    if (_systems.find(typeId) == _systems.end()) {
      LOG_DEBUG("[SystemManager] System used before registered.");
    }

    _signatures[typeId] = signature;
  }

  void destroyEntity(Entity entity) {
    for (auto &[_, system] : _systems) {
      system->_entities.erase(entity);
    }
  }

  void entitySignatureChanged(Entity entity, Signature entitySignature) {
    for (auto &[type, system] : _systems) {
      const auto &systemSignature = _signatures[type];

      if ((entitySignature & systemSignature) == systemSignature) {
        system->_entities.insert(entity);
      } else {
        system->_entities.erase(entity);
      }
    }
  }

private:
  std::unordered_map<std::type_index, Signature> _signatures{};
  std::unordered_map<std::type_index, std::shared_ptr<System>> _systems{};
};

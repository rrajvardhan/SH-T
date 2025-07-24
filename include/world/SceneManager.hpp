#pragma once

#include "Overseer.hpp"
#include <string>

class SceneManager
{
public:
  SceneManager();
  ~SceneManager();

  void loadScene(const std::string& path, Overseer& ecs);
  void reset(Overseer& ecs) { loadScene(_currentScene, ecs); }
  void clearAll(Overseer& ecs)
  {
    std::vector<Entity> toDestroy;
    for (Entity e : ecs.getEntities())
      toDestroy.push_back(e);
    for (Entity e : toDestroy)
      ecs.destroyEntity(e);
  }

  const std::string& getCurrentScene() const { return _currentScene; }

private:
  std::string _currentScene;
};

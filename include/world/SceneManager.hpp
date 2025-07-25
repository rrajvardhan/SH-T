#pragma once

#include "Json.hpp"
#include "Overseer.hpp"
#include <string>

struct Scene
{
  std::string name;
  std::string path;
  JSON        data;
};

class SceneManager
{
public:
  SceneManager();
  ~SceneManager();

  void loadScene(const std::string& path, Overseer& ecs);
  bool addScene(const std::string& path);
  void reset(Overseer& ecs)
  {
    if (_currentScene.name.empty())
      return;
    loadScene(_currentScene.name, ecs);
  }

private:
  Scene                                  _currentScene;
  std::unordered_map<std::string, Scene> _sceneCache;
};

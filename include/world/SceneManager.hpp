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
  SceneManager(Overseer& ecs);
  ~SceneManager();

  void loadScene(const std::string& path);
  bool addScene(const std::string& path);
  bool unload(const std::string& name);
  void reset()
  {
    if (_currentScene.name.empty())
      return;
    loadScene(_currentScene.name);
  }

  Scene& getCurrentScene() { return _currentScene; }

private:
  Overseer&                              _ecs;
  Scene                                  _currentScene;
  std::unordered_map<std::string, Scene> _sceneCache;
};

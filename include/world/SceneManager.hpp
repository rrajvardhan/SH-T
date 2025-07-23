#pragma once

#include "Overseer.hpp"
#include <string>

class SceneManager
{
public:
  SceneManager();
  ~SceneManager();

  void loadScene(const std::string& path, Overseer& ecs);
};

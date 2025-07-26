#pragma once

#include <SceneManager.hpp>
#include <string>
#include <vector>

class SceneBrowser
{
public:
  SceneBrowser(const std::string& sceneDir, SceneManager& scenes);

  void draw();
  void reload() { loadScenes(); }

private:
  std::string              _sceneDir;
  std::vector<std::string> _sceneFiles;
  std::string              _selectedScene;
  int                      _selectedIndex     = -1;
  int                      _renameIndex       = -1;
  char                     _renameBuffer[128] = {};
  bool                     _loaded            = false;

  SceneManager& _scenes;

  void loadScenes();
  void copyToAssetFolder(const std::string& srcPath, const std::string& destPath);
};

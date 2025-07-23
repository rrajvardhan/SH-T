#pragma once

#include "GlobalScriptSystem.hpp"
#include "TextEditor.h"
#include <imgui.h>
#include <string>
#include <vector>

class ScriptBrowser
{
public:
  ScriptBrowser(const std::string& assetDir, GlobalScriptSystem& scripts);
  void draw();

private:
  void loadScripts();
  void copyToAssetFolder(const std::string& srcPath, const std::string& destPath);
  void writeScriptToFile(const std::string& path, const std::string& content);

  TextEditor          _textEditor;
  GlobalScriptSystem& _scripts;

  std::string              _assetDir;
  std::vector<std::string> _scriptIDs;

  int         _selectedIndex = -1;
  int         _renameIndex   = -1;
  char        _renameBuffer[256]{};
  std::string _scriptContent;
  std::string _selectedScript;

  bool _loaded = false;
};

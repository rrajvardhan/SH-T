
#include "ImGuiFileDialog.h"
#include "SceneBrowser.hpp"
#include "imgui.h"
#include <filesystem>
#include <fstream>
#include <iostream>

SceneBrowser::SceneBrowser(const std::string& sceneDir, SceneManager& scenes)
    : _sceneDir(sceneDir), _scenes(scenes)
{
  loadScenes();
  _loaded = true;
}

void
SceneBrowser::loadScenes()
{
  _sceneFiles.clear();
  for (const auto& entry : std::filesystem::directory_iterator(_sceneDir))
  {
    if (!entry.is_regular_file())
      continue;
    if (entry.path().extension() == ".json")
    {
      _sceneFiles.push_back(entry.path().filename().string());
      _scenes.addScene(entry.path());
    }
  }
}

void
SceneBrowser::draw()
{
  if (!_loaded)
  {
    loadScenes();
    _loaded = true;
  }

  ImGui::Begin("SCENES");

  float padding    = 8.f;
  float thumbSize  = 64.f;
  float cellSize   = thumbSize + padding;
  float splitWidth = ImGui::GetContentRegionAvail().x;
  float gridWidth  = splitWidth * 0.3f;

  ImGui::BeginChild("SceneGrid", ImVec2(gridWidth, 0), true);

  if (ImGui::Button("Import"))
    ImGuiFileDialog::Instance()->OpenDialog("ChooseScene", "Choose Scene", ".json");

  if (ImGuiFileDialog::Instance()->Display("ChooseScene"))
  {
    if (ImGuiFileDialog::Instance()->IsOk())
    {
      std::string srcPath  = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string filename = ImGuiFileDialog::Instance()->GetCurrentFileName();
      std::string destPath = _sceneDir + "/" + filename;

      copyToAssetFolder(srcPath, destPath);
      _loaded = false;
    }
    ImGuiFileDialog::Instance()->Close();
  }

  ImGui::SameLine();
  if (ImGui::Button("+"))
    ImGui::OpenPopup("NewScenePopup");

  if (ImGui::BeginPopup("NewScenePopup"))
  {
    static char newSceneName[128] = "new_scene.json";
    ImGui::InputText("##SceneName", newSceneName, sizeof(newSceneName));
    if (ImGui::Button("Create"))
    {
      std::string newPath = _sceneDir + "/" + newSceneName;
      if (!std::filesystem::exists(newPath))
      {
        std::ofstream file(newPath);
        file << "{}"; // Default empty scene
        file.close();
        _loaded = false;
      }
      ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
      ImGui::CloseCurrentPopup();
    ImGui::EndPopup();
  }

  ImGui::Separator();

  int columns = std::max(1, int(gridWidth / cellSize));
  ImGui::Columns(columns, nullptr, false);

  for (int i = 0; i < static_cast<int>(_sceneFiles.size());)
  {
    ImGui::PushID(i);
    const std::string& scene = _sceneFiles[i];

    if (ImGui::Button("json", ImVec2(thumbSize, thumbSize)))
    {
      _selectedIndex        = i;
      _selectedScene        = scene;
      std::string sceneName = std::filesystem::path(scene).stem().string();
      _scenes.loadScene(sceneName);
    }

    if (ImGui::BeginPopupContextItem())
    {
      _selectedIndex = i;
      if (ImGui::Selectable("Rename"))
      {
        _renameIndex = i;
        snprintf(_renameBuffer, sizeof(_renameBuffer), "%s", scene.c_str());
      }

      if (ImGui::Selectable("Delete"))
      {
        std::filesystem::remove(_sceneDir + "/" + scene);
        if (_selectedIndex == i)
        {
          _selectedScene.clear();
        }
        loadScenes();
        _renameIndex = -1;
        ImGui::EndPopup();
        ImGui::PopID();
        continue;
      }
      ImGui::EndPopup();
    }

    if (_renameIndex == i)
    {
      if (ImGui::InputText("##Rename",
                           _renameBuffer,
                           sizeof(_renameBuffer),
                           ImGuiInputTextFlags_EnterReturnsTrue))
      {
        std::string oldPath = _sceneDir + "/" + scene;
        std::string newName = _renameBuffer;
        std::string newPath = _sceneDir + "/" + newName;

        if (newName != scene && !std::filesystem::exists(newPath))
        {
          std::filesystem::rename(oldPath, newPath);
          _sceneFiles[i] = newName;
          if (_selectedScene == scene)
            _selectedScene = newName;
        }

        _renameIndex = -1;
      }

      if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        _renameIndex = -1;
    }
    else
    {
      ImGui::TextWrapped("%s", scene.c_str());
    }

    ImGui::NextColumn();
    ImGui::PopID();
    ++i;
  }

  ImGui::Columns(1);
  ImGui::EndChild();

  ImGui::SameLine();
  ImGui::BeginChild("SceneDetails", ImVec2(0, 0), true);
  if (!_selectedScene.empty())
  {
    ImGui::Text("Loaded: %s", _selectedScene.c_str());
    ImGui::Text("-- TODO: add some details or preview --");
  }
  ImGui::EndChild();

  ImGui::End();
}

void
SceneBrowser::copyToAssetFolder(const std::string& srcPath, const std::string& destPath)
{
  if (!std::filesystem::exists(srcPath))
    return;
  std::filesystem::create_directories(std::filesystem::path(destPath).parent_path());
  std::ifstream src(srcPath, std::ios::binary);
  std::ofstream dst(destPath, std::ios::binary);
  if (!src || !dst)
    return;
  dst << src.rdbuf();
}

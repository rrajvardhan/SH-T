#include "ScriptBrowser.hpp"
#include "ImGuiFileDialog.h"
#include "imgui.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

ScriptBrowser::ScriptBrowser(const std::string& assetDir, GlobalScriptSystem& scripts)
    : _scripts(scripts), _assetDir(assetDir)
{
  auto lang = TextEditor::LanguageDefinition::Lua();
  _textEditor.SetLanguageDefinition(lang);
  _textEditor.SetShowWhitespaces(false);
  _textEditor.SetText(_scriptContent);
}

std::string
trimTrailingNewlines(const std::string& text)
{
  size_t end = text.find_last_not_of("\n\r");
  if (end == std::string::npos)
    return "";
  return text.substr(0, end + 1);
}

void
ScriptBrowser::writeScriptToFile(const std::string& path, const std::string& content)
{
  std::ofstream out(path, std::ios::trunc);
  if (!out)
  {
    std::cerr << "Failed to write: " << path << "\n";
    return;
  }

  out << content;
}

void
ScriptBrowser::loadScripts()
{
  _scriptIDs.clear();

  for (const auto& file : std::filesystem::directory_iterator(_assetDir))
  {
    if (!file.is_regular_file())
      continue;

    auto path = file.path();
    if (path.extension() != ".lua")
      continue;

    _scriptIDs.push_back(path.filename().string());
  }
}

void
ScriptBrowser::draw()
{
  if (!_loaded)
  {
    loadScripts();
    _loaded = true;
  }

  if (_reload)
  {
    _scripts.reload();
    _reload = false;
  }

  ImGui::Begin("SCRIPTS");

  float scriptListWidth = 300.0f;
  ImGui::BeginChild("ScriptList", ImVec2(scriptListWidth, 0), true);

  if (ImGui::Button("Import"))
    ImGuiFileDialog::Instance()->OpenDialog("ChooseLua", "Choose Lua File", ".lua");

  if (ImGuiFileDialog::Instance()->Display("ChooseLua"))
  {
    if (ImGuiFileDialog::Instance()->IsOk())
    {
      std::string srcPath  = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string filename = ImGuiFileDialog::Instance()->GetCurrentFileName();
      std::string destPath = _assetDir + "/" + filename;
      copyToAssetFolder(srcPath, destPath);
      _loaded = false;
    }
    ImGuiFileDialog::Instance()->Close();
  }
  ImGui::SameLine();
  if (ImGui::Button("+"))
  {
    ImGui::OpenPopup("NewScriptPopup");
  }
  if (ImGui::BeginPopup("NewScriptPopup"))
  {
    static char newScriptName[128] = "new_script.lua";
    ImGui::InputText("##ScriptName", newScriptName, sizeof(newScriptName));
    if (ImGui::Button("Create"))
    {
      std::string newPath = _assetDir + "/" + newScriptName;
      if (!std::filesystem::exists(newPath))
      {
        std::ofstream file(newPath);
        file << "-- " << newScriptName << "\n\n";
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
  for (int i = 0; i < (int) _scriptIDs.size(); ++i)
  {
    ImGui::PushID(i);
    const std::string& script = _scriptIDs[i];

    if (ImGui::Selectable(script.c_str(), i == _selectedIndex))
    {
      _selectedIndex  = i;
      _selectedScript = script;

      std::ifstream file(_assetDir + "/" + script);
      _scriptContent
          = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      _textEditor.SetText(_scriptContent);
    }

    if (ImGui::BeginPopupContextItem())
    {
      _renameIndex = i;
      strncpy(_renameBuffer, script.c_str(), sizeof(_renameBuffer));

      if (ImGui::Selectable("Delete"))
      {
        std::filesystem::remove(_assetDir + "/" + script);
        if (_selectedIndex == i)
        {
          _selectedScript.clear();
          _scriptContent.clear();
        }
        loadScripts();
        _renameIndex = -1;
        ImGui::EndPopup();
        ImGui::PopID();
        continue;
      }
      ImGui::EndPopup();
    }

    ImGui::PopID();
  }

  ImGui::EndChild();
  ImGui::SameLine();

  ImGui::BeginChild("ScriptEditor", ImVec2(0, 0), true);
  if (!_selectedScript.empty())
  {
    const std::string filename = _assetDir + "/" + _selectedScript;
    ImGui::Text("%s", _selectedScript.c_str());

    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 50);

    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)
        && (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_S)))
    {
      _scriptContent = trimTrailingNewlines(_textEditor.GetText());
      writeScriptToFile(filename, _scriptContent);
      _reload = true;
    }

    if (ImGui::Button("Save"))
    {
      _scriptContent = trimTrailingNewlines(_textEditor.GetText());
      writeScriptToFile(filename, _scriptContent);
      _reload = true;
    }

    _textEditor.Render("Script Editor");
  }

  ImGui::EndChild();
  ImGui::End();
}

void
ScriptBrowser::copyToAssetFolder(const std::string& srcPath, const std::string& destPath)
{
  if (!std::filesystem::exists(srcPath))
  {
    return;
  }
  std::filesystem::create_directories(std::filesystem::path(destPath).parent_path());
  std::ifstream src(srcPath, std::ios::binary);
  std::ofstream dst(destPath, std::ios::binary);

  if (!src || !dst)
  {
    return;
  }

  dst << src.rdbuf();
}

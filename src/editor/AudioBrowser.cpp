#include "AudioBrowser.hpp"
#include "AudioManager.hpp"
#include "ImGuiFileDialog.h"
#include "JsonSerializer.hpp"
#include "imgui.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>

AudioBrowser::AudioBrowser(const std::string& assetDir, AudioManager& audioManager)
    : _assetDir(assetDir), _audioManager(audioManager)
{
}

void
AudioBrowser::loadAudioFiles()
{
  _musicIDs.clear();
  _sfxIDs.clear();

  for (const auto& file : std::filesystem::directory_iterator(_assetDir))
  {
    if (!file.is_regular_file())
      continue;

    auto path = file.path();
    if (path.extension() != ".wav" && path.extension() != ".ogg")
      continue;

    std::string name = path.stem().string();

    if (name.rfind("bgm_", 0) == 0)
    {
      _musicIDs.push_back(name);
      _audioManager.addMusic(name, path);
    }
    else if (name.rfind("sfx_", 0) == 0)
    {
      _sfxIDs.push_back(name);
      _audioManager.addSFX(name, path);
    }
    else
    {
      _sfxIDs.push_back(name);
      _audioManager.addSFX(name, path);
    }

    if (_musicVolumes.find(name) == _musicVolumes.end())
      _musicVolumes[name] = 128;
    if (_sfxVolumes.find(name) == _sfxVolumes.end())
      _sfxVolumes[name] = 128;
  }
}

void
AudioBrowser::draw()
{
  if (!_loaded)
  {
    loadAudioFiles();
    _loaded = true;
  }

  ImGui::Begin("SOUNDS");

  if (!_selectedAudio.empty())
  {
    bool isMusic = std::find(_musicIDs.begin(), _musicIDs.end(), _selectedAudio) != _musicIDs.end();
    drawControls(_selectedAudio, isMusic);
  }

  float totalWidth = ImGui::GetContentRegionAvail().x - 10;
  float halfWidth  = totalWidth * 0.5f;
  float cellSize   = 64.0f + 8.0f;

  int sfxColumns   = std::max(1, int(halfWidth / cellSize));
  int musicColumns = sfxColumns;

  drawAudioGrid("sfx", _sfxIDs, sfxColumns, halfWidth);
  ImGui::SameLine();
  drawAudioGrid("music", _musicIDs, musicColumns, totalWidth - halfWidth);

  ImGui::End();
}

void
AudioBrowser::drawAudioGrid(const char*               label,
                            std::vector<std::string>& audioList,
                            int                       columns,
                            float                     panelWidth)
{

  bool isMusic = (std::string(label) == "music");

  std::string importDialogKey   = isMusic ? "ChooseMusic" : "ChooseSFX";
  std::string importButtonLabel = isMusic ? "+" : "+";

  ImGui::BeginChild(label, ImVec2(panelWidth, 0), true);
  if (ImGui::Button(importButtonLabel.c_str()))
  {
    ImGuiFileDialog::Instance()->OpenDialog(importDialogKey, "Select Audio File", ".wav,.ogg");
  }
  if (ImGuiFileDialog::Instance()->Display(importDialogKey))
  {
    if (ImGuiFileDialog::Instance()->IsOk())
    {
      std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
      std::string rawName      = ImGuiFileDialog::Instance()->GetCurrentFileName();
      std::string prefix       = isMusic ? "bgm_" : "sfx_";

      if (rawName.rfind(prefix, 0) != 0)
        rawName = prefix + rawName;

      std::string destPath = _assetDir + "/" + rawName;
      copyToAssetFolder(filePathName, destPath);

      loadAudioFiles();
    }
    ImGuiFileDialog::Instance()->Close();
  }
  ImGui::SameLine();
  ImGui::Text("%s", label);

  ImGui::Separator();
  ImGui::Columns(columns, nullptr, false);

  for (int i = 0; i < static_cast<int>(audioList.size());)
  {
    ImGui::PushID((std::string(label) + std::to_string(i)).c_str());
    const std::string& audio = audioList[i];

    if (ImGui::Button(label, ImVec2(64, 64)))
    {
      _selectedAudio = audio;
    }

    if (ImGui::BeginPopupContextItem())
    {
      if (ImGui::Selectable("Delete"))
      {
        deleteAudio(_selectedAudio, isMusic);
        _selectedAudio.clear();
        _renameIndex = -1;
        ImGui::EndPopup();
        ImGui::PopID();
        continue;
      }
      ImGui::EndPopup();
    }

    if (_renameIndex == i)
    {
    }
    else
    {
      ImGui::TextWrapped("%s", audio.c_str());
    }

    ImGui::NextColumn();
    ImGui::PopID();
    ++i;
  }

  ImGui::Columns(1);
  ImGui::EndChild();
}

void
AudioBrowser::deleteAudio(std::string& id, bool isMusic)
{
  static const std::vector<std::string> extensions = { ".ogg", ".wav" };

  std::filesystem::path fileToDelete;
  for (const auto& ext : extensions)
  {
    std::filesystem::path testPath = _assetDir + "/" + (id + ext);
    if (std::filesystem::exists(testPath))
    {
      fileToDelete = testPath;
      break;
    }
  }

  if (!fileToDelete.empty())
  {
    std::filesystem::remove(fileToDelete);
  }

  auto& list = isMusic ? _musicIDs : _sfxIDs;
  list.erase(std::remove(list.begin(), list.end(), id), list.end());

  if (isMusic)
    _audioManager.unloadMusic(id);
  else
    _audioManager.unloadSFX(id);

  _selectedAudio.clear();
  _renameIndex = -1;
}

void
AudioBrowser::drawControls(const std::string& id, bool isMusic)
{
  ImGui::PushID(id.c_str());

  if (ImGui::Button("Play"))
  {
    if (isMusic)
    {
      _audioManager.playMusic(id);
      _musicState = PlaybackState::Playing;
    }
    else
    {
      _audioManager.playSFX(id);
      _sfxState = PlaybackState::Playing;
    }
  }

  if (isMusic && _musicState == PlaybackState::Playing)
  {
    ImGui::SameLine();
    if (ImGui::Button("Pause"))
    {
      _audioManager.pauseMusic();
      _musicState = PlaybackState::Paused;
    }
  }

  if (isMusic && _musicState == PlaybackState::Paused)
  {
    ImGui::SameLine();
    if (ImGui::Button("Resume"))
    {
      _audioManager.resumeMusic();
      _musicState = PlaybackState::Playing;
    }
  }

  if (isMusic)
  {
    ImGui::SameLine();
    if (ImGui::Button("Stop"))
    {
      _audioManager.stopMusic();
      _musicState = PlaybackState::Stopped;
    }
  }

  int* vol = isMusic ? &_musicVolumes[id] : &_sfxVolumes[id];

  ImGui::SameLine();
  if (ImGui::SliderInt("Volume", vol, 0, 128))
  {
    if (isMusic)
      _audioManager.setMusicVolume(*vol);
    else
      _audioManager.setSFXVolume(id, *vol);
  }

  ImGui::PopID();
}

void
AudioBrowser::copyToAssetFolder(const std::string& srcPath, const std::string& destPath)
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

void
AudioBrowser::serialize(JSONSerializer& serializer)
{

  serializer.StartNewObject("musicVolumes");
  for (const auto& [id, vol] : _musicVolumes)
    serializer.AddKeyValuePair(id, vol);
  serializer.EndObject();

  serializer.StartNewObject("sfxVolumes");
  for (const auto& [id, vol] : _sfxVolumes)
    serializer.AddKeyValuePair(id, vol);
  serializer.EndObject();
}

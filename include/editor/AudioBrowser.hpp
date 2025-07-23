#pragma once

#include "JsonSerializer.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class AudioManager;

class AudioBrowser
{
public:
  AudioBrowser(const std::string& assetDir, AudioManager& audioManager);
  void draw();
  void serialize(JSONSerializer& serializer);

private:
  void loadAudioFiles();
  void copyToAssetFolder(const std::string& srcPath, const std::string& destPath);
  void drawAudioGrid(const char*               label,
                     std::vector<std::string>& audioList,
                     int                       columns,
                     float                     panelWidth);
  void drawControls(const std::string& id, bool isMusic);
  void deleteAudio(std::string& id, bool isMusic);

  enum class PlaybackState
  {
    Stopped,
    Playing,
    Paused
  };

  PlaybackState _musicState = PlaybackState::Stopped;
  PlaybackState _sfxState   = PlaybackState::Stopped;

  std::string              _assetDir;
  std::vector<std::string> _musicIDs;
  std::vector<std::string> _sfxIDs;
  int                      _selectedIndex     = -1;
  int                      _renameIndex       = -1;
  char                     _renameBuffer[128] = {};
  bool                     _loaded            = false;

  std::string   _selectedAudio;
  AudioManager& _audioManager;

  std::unordered_map<std::string, int> _musicVolumes;
  std::unordered_map<std::string, int> _sfxVolumes;
};

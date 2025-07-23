#pragma once

#include <SDL2/SDL_mixer.h>
#include <algorithm>
#include <string>
#include <unordered_map>

class AudioManager
{
public:
  AudioManager();
  ~AudioManager();

  // Music
  void addMusic(const std::string& id, const std::string& path);
  void playMusic(const std::string& id, int loops = -1); // -1 = infinite
  void stopMusic();
  void pauseMusic();
  void resumeMusic();
  void unloadMusic(const std::string& id);
  bool hasMusic(const std::string& id) const;

  // Sound effects
  void addSFX(const std::string& id, const std::string& path);
  void playSFX(const std::string& id, int loops = 0); // 0 = once
  void unloadSFX(const std::string& id);
  bool hasSFX(const std::string& id) const;

  void renameAudio(const std::string& oldID, const std::string& newID, bool isMusic);

  void setMusicVolume(int vol)
  {
    _musicVolume = std::clamp(vol, 0, 128);
    Mix_VolumeMusic(_musicVolume);
  }

  void setSFXVolume(int vol)
  {
    _sfxVolume = std::clamp(vol, 0, 128);
    for (auto& [id, chunk] : _sfx)
    {
      Mix_VolumeChunk(chunk, _sfxVolume);
    }
  }

  void setSFXVolume(const std::string& id, int volume)
  {
    auto it = _sfx.find(id);
    if (it != _sfx.end())
      Mix_VolumeChunk(it->second, volume);
  }

private:
  std::unordered_map<std::string, Mix_Music*> _music;
  std::unordered_map<std::string, Mix_Chunk*> _sfx;

  int _musicVolume = 128;
  int _sfxVolume   = 128;

  bool _initialized = false;
};

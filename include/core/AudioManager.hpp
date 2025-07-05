#pragma once

#include <SDL2/SDL_mixer.h>
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

private:
  std::unordered_map<std::string, Mix_Music*> _music;
  std::unordered_map<std::string, Mix_Chunk*> _sfx;

  bool _initialized = false;
};

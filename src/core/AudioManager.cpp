#include "AudioManager.hpp"
#include "Log.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

AudioManager::AudioManager() {
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
    LOG_ERROR("[AudioManager] Failed to initialize SDL_mixer: ", Mix_GetError());
    return;
  }
  _initialized = true;
  LOG_SUCCESS("[AudioManager] SDL_mixer initialized.");
}

AudioManager::~AudioManager() {
  for (auto &[id, music] : _music) {
    if (music)
      Mix_FreeMusic(music);
  }
  _music.clear();

  for (auto &[id, chunk] : _sfx) {
    if (chunk)
      Mix_FreeChunk(chunk);
  }
  _sfx.clear();

  if (_initialized) {
    Mix_CloseAudio();
    Mix_Quit();
    LOG_INFO("[AudioManager] SDL_mixer shut down.");
  }
}

void AudioManager::addMusic(const std::string &id, const std::string &path) {

  char *basePath = SDL_GetBasePath();
  if (!basePath) {
    LOG_ERROR("Failed to get base path: ", SDL_GetError());
    return;
  }

  std::string fullpath = std::string(basePath) + path;
  SDL_free(basePath);

  Mix_Music *music = Mix_LoadMUS(fullpath.c_str());
  if (!music) {
    LOG_ERROR("[AudioManager] Failed to load music: ", path, " | ", Mix_GetError());
    return;
  }

  auto it = _music.find(id);
  if (it != _music.end() && it->second) {
    Mix_FreeMusic(it->second); // Avoid leaks
  }

  _music[id] = music;
  LOG_SUCCESS("[AudioManager] Music loaded with ID: ", id);
}

void AudioManager::playMusic(const std::string &id, int loops) {
  auto it = _music.find(id);
  if (it == _music.end()) {
    LOG_ERROR("[AudioManager] Music ID not found: ", id);
    return;
  }

  if (Mix_PlayMusic(it->second, loops) == -1) {
    LOG_ERROR("[AudioManager] Failed to play music: ", Mix_GetError());
  }
}

void AudioManager::stopMusic() { Mix_HaltMusic(); }

void AudioManager::pauseMusic() {
  if (Mix_PlayingMusic())
    Mix_PauseMusic();
}

void AudioManager::resumeMusic() {
  if (Mix_PausedMusic())
    Mix_ResumeMusic();
}

void AudioManager::unloadMusic(const std::string &id) {
  auto it = _music.find(id);
  if (it != _music.end()) {
    if (it->second)
      Mix_FreeMusic(it->second);
    _music.erase(it);
    LOG_INFO("[AudioManager] Unloaded music ID: ", id);
  }
}

bool AudioManager::hasMusic(const std::string &id) const { return _music.find(id) != _music.end(); }

void AudioManager::addSFX(const std::string &id, const std::string &path) {

  char *basePath = SDL_GetBasePath();
  if (!basePath) {
    LOG_ERROR("Failed to get base path: ", SDL_GetError());
    return;
  }

  std::string fullpath = std::string(basePath) + path;
  SDL_free(basePath);

  Mix_Chunk *chunk = Mix_LoadWAV(fullpath.c_str());
  if (!chunk) {
    LOG_ERROR("[AudioManager] Failed to load SFX: ", path, " | ", Mix_GetError());
    return;
  }

  auto it = _sfx.find(id);
  if (it != _sfx.end() && it->second) {
    Mix_FreeChunk(it->second);
  }

  _sfx[id] = chunk;
  LOG_SUCCESS("[AudioManager] SFX loaded with ID: ", id);
}

void AudioManager::playSFX(const std::string &id, int loops) {
  auto it = _sfx.find(id);
  if (it == _sfx.end()) {
    LOG_ERROR("[AudioManager] SFX ID not found: ", id);
    return;
  }

  if (Mix_PlayChannel(-1, it->second, loops) == -1) {
    LOG_ERROR("[AudioManager] Failed to play SFX: ", Mix_GetError());
  }
}

void AudioManager::unloadSFX(const std::string &id) {
  auto it = _sfx.find(id);
  if (it != _sfx.end()) {
    if (it->second)
      Mix_FreeChunk(it->second);
    _sfx.erase(it);
    LOG_INFO("[AudioManager] Unloaded SFX ID: ", id);
  }
}

bool AudioManager::hasSFX(const std::string &id) const { return _sfx.find(id) != _sfx.end(); }

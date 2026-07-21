#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() {}

AudioManager::~AudioManager() {
    clean();
}

bool AudioManager::init() {
    if (!MIX_Init()) {
        std::cerr << "Khong the khoi tao SDL3_mixer: " << SDL_GetError() << std::endl;
        return false;
    }

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!mixer) {
        std::cerr << "Khong the tao mixer device: " << SDL_GetError() << std::endl;
        return false;
    }

    menuMusicTrack = MIX_CreateTrack(mixer);
    gameMusicTrack = MIX_CreateTrack(mixer);
    jumpTrack = MIX_CreateTrack(mixer);
    hitTrack = MIX_CreateTrack(mixer);

    return true;
}

void AudioManager::loadMenuMusic(const char* path) {
    menuMusicAudio = MIX_LoadAudio(mixer, path, false);
    if (!menuMusicAudio) {
        std::cerr << "Khong load duoc nhac menu: " << SDL_GetError() << std::endl;
        return;
    }
    MIX_SetTrackAudio(menuMusicTrack, menuMusicAudio);
}

void AudioManager::loadGameMusic(const char* path) {
    gameMusicAudio = MIX_LoadAudio(mixer, path, false);
    if (!gameMusicAudio) {
        std::cerr << "Khong load duoc nhac choi game: " << SDL_GetError() << std::endl;
        return;
    }
    MIX_SetTrackAudio(gameMusicTrack, gameMusicAudio);
}

void AudioManager::playMenuMusic() {
    if (!menuMusicTrack) return;

    // Neu nhac menu dang phat roi thi khong lam gi ca
    if (MIX_TrackPlaying(menuMusicTrack)) return;

    // Dung nhac game truoc khi phat nhac menu
    if (gameMusicTrack) MIX_StopTrack(gameMusicTrack, 0);

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_PlayTrack(menuMusicTrack, props);
    SDL_DestroyProperties(props);
}

void AudioManager::playGameMusic() {
    if (!gameMusicTrack) return;

    if (MIX_TrackPlaying(gameMusicTrack)) return;

    if (menuMusicTrack) MIX_StopTrack(menuMusicTrack, 0);

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_PlayTrack(gameMusicTrack, props);
    SDL_DestroyProperties(props);
}

void AudioManager::stopAllMusic() {
    if (menuMusicTrack) MIX_StopTrack(menuMusicTrack, 0);
    if (gameMusicTrack) MIX_StopTrack(gameMusicTrack, 0);
}

void AudioManager::loadSfxJump(const char* path) {
    jumpAudio = MIX_LoadAudio(mixer, path, true);
    if (!jumpAudio) {
        std::cerr << "Khong load duoc am thanh nhay: " << SDL_GetError() << std::endl;
        return;
    }
    MIX_SetTrackAudio(jumpTrack, jumpAudio);
}

void AudioManager::loadSfxHit(const char* path) {
    hitAudio = MIX_LoadAudio(mixer, path, true);
    if (!hitAudio) {
        std::cerr << "Khong load duoc am thanh va cham: " << SDL_GetError() << std::endl;
        return;
    }
    MIX_SetTrackAudio(hitTrack, hitAudio);
}

void AudioManager::playJump() {
    if (jumpTrack) MIX_PlayTrack(jumpTrack, 0);
}

void AudioManager::playHit() {
    if (hitTrack) MIX_PlayTrack(hitTrack, 0);
}

void AudioManager::clean() {
    if (menuMusicAudio) { MIX_DestroyAudio(menuMusicAudio); menuMusicAudio = nullptr; }
    if (gameMusicAudio) { MIX_DestroyAudio(gameMusicAudio); gameMusicAudio = nullptr; }
    if (jumpAudio) { MIX_DestroyAudio(jumpAudio); jumpAudio = nullptr; }
    if (hitAudio) { MIX_DestroyAudio(hitAudio); hitAudio = nullptr; }

    if (mixer) {
        MIX_DestroyMixer(mixer);
        mixer = nullptr;
    }

    MIX_Quit();
}

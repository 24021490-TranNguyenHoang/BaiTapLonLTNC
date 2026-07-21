#ifndef AUDIOMANAGER_H_INCLUDED
#define AUDIOMANAGER_H_INCLUDED

#include <SDL3_mixer/SDL_mixer.h>

class AudioManager {
public:
    AudioManager();
    ~AudioManager();

    bool init();

    void loadMenuMusic(const char* path);
    void loadGameMusic(const char* path);
    void playMenuMusic();
    void playGameMusic();
    void stopAllMusic();

    void loadSfxJump(const char* path);
    void loadSfxHit(const char* path);
    void playJump();
    void playHit();

    void clean();

private:
    MIX_Mixer* mixer = nullptr;

    MIX_Audio* menuMusicAudio = nullptr;
    MIX_Track* menuMusicTrack = nullptr;

    MIX_Audio* gameMusicAudio = nullptr;
    MIX_Track* gameMusicTrack = nullptr;

    MIX_Audio* jumpAudio = nullptr;
    MIX_Track* jumpTrack = nullptr;

    MIX_Audio* hitAudio = nullptr;
    MIX_Track* hitTrack = nullptr;
};

#endif // AUDIOMANAGER_H_INCLUDED

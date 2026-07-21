#ifndef SHURIKEN_H_INCLUDED
#define SHURIKEN_H_INCLUDED

#include <SDL3/SDL.h>

const int SHURIKEN_FRAME_COUNT = 4;

class Shuriken {
public:
    Shuriken();

    void init(SDL_Texture* frames[SHURIKEN_FRAME_COUNT]);
    void update(float deltaTime, float speed);
    void render(SDL_Renderer* renderer);

    SDL_FRect getHitbox() const;
    bool isOffScreen() const;

    float x, y;   // y co dinh, khong doi trong suot vong doi shuriken
    int w, h;

private:
    SDL_Texture* frameTextures[SHURIKEN_FRAME_COUNT] = { nullptr };

    int currentFrame = 0;
    float frameTimer = 0.0f;
    float frameDuration = 0.06f;
    static constexpr float speedMultiplier = 1.8f;
};

#endif // SHURIKEN_H_INCLUDED

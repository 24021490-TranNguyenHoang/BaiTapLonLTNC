#ifndef OBSTACLE_H_INCLUDED
#define OBSTACLE_H_INCLUDED

#include <SDL3/SDL.h>

const int SPEAR_FRAME_COUNT = 12;

class Obstacle {
public:
    Obstacle();
    // Khong co destructor tu huy texture nua, vi texture dung chung, khong thuoc so huu rieng

    void init(SDL_Texture* sharedTexture, int frameW, int frameH);
    void update(float deltaTime, float speed);
    void render(SDL_Renderer* renderer);

    SDL_FRect getHitbox() const;
    bool isOffScreen() const;

    float x, y;
    int w, h;

private:
    SDL_Texture* spriteSheet = nullptr; // con tro toi texture dung chung, khong tu huy
    int frameWidth = 0;
    int frameHeight = 0;

    int currentFrame = 0;
    float frameTimer = 0.0f;
    float frameDuration = 0.08f;

    float hitboxHeightRatio[SPEAR_FRAME_COUNT] = {
        0.23f, 0.25f, 1.0f, 1.0f, 1.0f, 1.0f, 0.97f, 0.69f, 1.0f, 1.0f, 1.0f, 1.0f
    };
};

#endif // OBSTACLE_H_INCLUDED

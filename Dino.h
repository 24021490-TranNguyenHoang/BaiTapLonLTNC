#ifndef DINO_H_INCLUDED
#define DINO_H_INCLUDED

#include <SDL3/SDL.h>

const int DINO_RUN_FRAME_COUNT = 8;

class Dino {
public:
    Dino();
    ~Dino();

    void init(SDL_Renderer* renderer);
    void handleInput(const bool* keystate);
    void update(int groundY, float deltaTime);
    void render(SDL_Renderer* renderer);
    void jump();
    void reset(int groundY);

    float getX() const { return x; }
    float getY() const { return y; }
    int getW() const { return w; }
    int getH() const { return h; }

private:
    float x, y;
    int w, h;
    float velocityY = 0;
    bool isGrounded = false;

    SDL_Texture* spriteSheet = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;

    int currentFrame = 0;
    float frameTimer = 0.0f;
    float frameDuration = 0.1f; // giay moi frame
};

#endif // DINO_H_INCLUDED

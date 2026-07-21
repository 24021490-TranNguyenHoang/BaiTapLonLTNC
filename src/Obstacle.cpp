#include "Obstacle.h"

Obstacle::Obstacle() {
    x = 0;
    y = 0;
    w = 9;
    h = 64;
}

void Obstacle::init(SDL_Texture* sharedTexture, int frameW, int frameH) {
    spriteSheet = sharedTexture;
    frameWidth = frameW;
    frameHeight = frameH;
}

void Obstacle::update(float deltaTime, float speed) {
    x -= speed;

    frameTimer += deltaTime;
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame = (currentFrame + 1) % SPEAR_FRAME_COUNT;
    }
}

void Obstacle::render(SDL_Renderer* renderer) {
    SDL_FRect src = {
        (float)(currentFrame * frameWidth),
        0.0f,
        (float)frameWidth,
        (float)frameHeight
    };
    SDL_FRect dst = { x, y, (float)w, (float)h };

    SDL_RenderTexture(renderer, spriteSheet, &src, &dst);
}

SDL_FRect Obstacle::getHitbox() const {
    float ratio = hitboxHeightRatio[currentFrame];
    float hitH = h * ratio;

    SDL_FRect box;
    box.x = x + 3;
    box.w = w - 6;
    box.h = hitH;
    box.y = y + (h - hitH);

    return box;
}

bool Obstacle::isOffScreen() const {
    return (x + w < 0);
}

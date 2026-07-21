#include "Shuriken.h"

Shuriken::Shuriken() {
    x = 0;
    y = 0;
    w = 32;
    h = 32;
}

void Shuriken::init(SDL_Texture* frames[SHURIKEN_FRAME_COUNT]) {
    for (int i = 0; i < SHURIKEN_FRAME_COUNT; i++) {
        frameTextures[i] = frames[i];
    }
}

void Shuriken::update(float deltaTime, float speed) {
    x -= speed * speedMultiplier;
    frameTimer += deltaTime;
    if (frameTimer >= frameDuration) {
        frameTimer = 0.0f;
        currentFrame = (currentFrame + 1) % SHURIKEN_FRAME_COUNT;
    }
}

void Shuriken::render(SDL_Renderer* renderer) {
    SDL_FRect dst = { x, y, (float)w, (float)h };
    SDL_RenderTexture(renderer, frameTextures[currentFrame], nullptr, &dst);
}

SDL_FRect Shuriken::getHitbox() const {
    SDL_FRect box;
    box.x = x + 4;
    box.y = y + 4;
    box.w = w - 8;
    box.h = h - 8;
    return box;
}

bool Shuriken::isOffScreen() const {
    return (x + w < 0);
}

#include "Dino.h"
#include <iostream>

const float GRAVITY = 0.6f;
const float JUMP_FORCE = -12.0f;

Dino::Dino() {
    x = 50;
    y = 300;
    w = 25;
    h = 36;
}

Dino::~Dino() {
    if (spriteSheet) {
        SDL_DestroyTexture(spriteSheet);
    }
}

void Dino::init(SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_LoadPNG("assets/dino_run_sheet_trimmed.png");
    if (!surface) {
        std::cerr << "Khong load duoc anh: " << SDL_GetError() << std::endl;
        return;
    }

    spriteSheet = SDL_CreateTextureFromSurface(renderer, surface);

    // Tu tinh kich thuoc 1 frame dua tren tong kich thuoc anh / so frame
    frameWidth = surface->w / DINO_RUN_FRAME_COUNT;
    frameHeight = surface->h;

    SDL_DestroySurface(surface);
}

void Dino::handleInput(const bool* keystate) {
    if (keystate[SDL_SCANCODE_SPACE] && isGrounded) {
        jump();
    }
}

void Dino::jump() {
    if (isGrounded) {
        velocityY = JUMP_FORCE;
        isGrounded = false;
    }
}

void Dino::update(int groundY, float deltaTime) {
    y += velocityY;
    velocityY += GRAVITY;

    if (y >= groundY - h) {
        y = groundY - h;
        velocityY = 0;
        isGrounded = true;
    }

    if (isGrounded) {
        frameTimer += deltaTime;
        if (frameTimer >= frameDuration) {
            frameTimer = 0.0f;
            currentFrame = (currentFrame + 1) % DINO_RUN_FRAME_COUNT;
        }
    }
}

void Dino::reset(int groundY) {
    y = groundY - h;
    velocityY = 0;
    isGrounded = true;
}

void Dino::render(SDL_Renderer* renderer) {
    SDL_FRect src = {
        (float)(currentFrame * frameWidth),
        0.0f,
        (float)frameWidth,
        (float)frameHeight
    };

    SDL_FRect dst = { x, y, (float)w, (float)h };

    SDL_RenderTexture(renderer, spriteSheet, &src, &dst);
}

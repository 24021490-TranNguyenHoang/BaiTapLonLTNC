#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include "Dino.h"
#include "Obstacle.h"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;
const int GROUND_Y = 300;

int main(int argc, char* argv[]) {
    // 1. Khoi tao SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Khong the khoi tao SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 2. Tao Window va Renderer
    SDL_Window* window = SDL_CreateWindow("DinoRun - SDL3", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!window || !renderer) {
        std::cerr << "Loi tao Window/Renderer: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 3. Khoi tao Dino
    Dino dino;
    dino.init(renderer);

    // 4. Load anh obstacle (spear) 1 LAN DUY NHAT o day
    SDL_Surface* spearSurface = SDL_LoadPNG("asset/sprites/spear_sheet_trimmed.png");
    SDL_Texture* spearTexture = nullptr;
    int spearFrameW = 0, spearFrameH = 0;

    if (!spearSurface) {
        std::cerr << "Khong load duoc anh spear: " << SDL_GetError() << std::endl;
    } else {
        spearTexture = SDL_CreateTextureFromSurface(renderer, spearSurface);
        spearFrameW = spearSurface->w / SPEAR_FRAME_COUNT;
        spearFrameH = spearSurface->h;
        SDL_DestroySurface(spearSurface);
    }

    // 5. Danh sach obstacle dang ton tai
    std::vector<Obstacle> obstacles;

    // 6. Khoi tao trang thai Game
    float gameSpeed = 5.0f;
    int score = 0;
    int spawnTimer = 0;
    bool isGameOver = false;
    bool running = true;
    SDL_Event event;

    Uint64 lastTime = SDL_GetTicks();

    // Game loop
    while (running) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_SPACE) {
                    if (isGameOver) {
                        dino.reset(GROUND_Y);
                        obstacles.clear();
                        score = 0;
                        gameSpeed = 5.0f;
                        isGameOver = false;
                    } else {
                        dino.jump();
                    }
                }
            }
        }

        if (!isGameOver) {
            dino.update(GROUND_Y, deltaTime);

            // Spawn obstacle moi - KHONG load anh lai, chi dung texture co san
            spawnTimer++;
            if (spawnTimer > 100 + (rand() % 80)) {
                Obstacle trap;
                trap.init(spearTexture, spearFrameW, spearFrameH);
                trap.x = (float)SCREEN_WIDTH;
                trap.y = GROUND_Y - trap.h;
                obstacles.push_back(trap);
                spawnTimer = 0;
            }

            for (size_t i = 0; i < obstacles.size(); ) {
                obstacles[i].update(deltaTime, gameSpeed);

                SDL_FRect dinoBox = { dino.getX() + 3, dino.getY() + 3, (float)dino.getW() - 6, (float)dino.getH() - 6 };
                SDL_FRect trapBox = obstacles[i].getHitbox();

                if (SDL_HasRectIntersectionFloat(&dinoBox, &trapBox)) {
                    isGameOver = true;
                }

                if (obstacles[i].isOffScreen()) {
                    obstacles.erase(obstacles.begin() + i);
                    score += 10;
                    gameSpeed += 0.1f;
                } else {
                    i++;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 247, 247, 247, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 83, 83, 83, 255);
        SDL_FRect groundRect = { 0, (float)GROUND_Y, (float)SCREEN_WIDTH, 2 };
        SDL_RenderFillRect(renderer, &groundRect);

        dino.render(renderer);

        for (auto& obs : obstacles) {
            obs.render(renderer);
        }

        if (isGameOver) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 50);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_FRect overlay = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // 7. Giai phong tai nguyen
    if (spearTexture) SDL_DestroyTexture(spearTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Tro choi ket thuc! Diem cua ban: " << score << std::endl;
    return 0;
}

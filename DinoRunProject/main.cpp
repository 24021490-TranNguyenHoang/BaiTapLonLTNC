#include <SDL3/SDL.h>
#include <iostream>
#include <vector>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 400;
const int GROUND_Y = 300;
const float GRAVITY = 0.6f;
const float JUMP_FORCE = -12.0f;

struct Entity {
    float x, y;
    int w, h;
    float velocityY = 0;
    bool isGrounded = false;
};

int main(int argc, char* argv[]) {
    // 1. Khoi tao SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Khong the khoi tao SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // 2. Tao Window và Renderer
    SDL_Window* window = SDL_CreateWindow("DinoRun - SDL3", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (!window || !renderer) {
        std::cerr << "Loi tao Window/Renderer: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 3. Tai hinh anh
    SDL_Surface* dinoSurface = SDL_LoadBMP("dino.bmp");
    SDL_Surface* cactusSurface = SDL_LoadBMP("cactus.bmp");

    SDL_Texture* dinoTexture = nullptr;
    SDL_Texture* cactusTexture = nullptr;

    if (dinoSurface) {
        dinoTexture = SDL_CreateTextureFromSurface(renderer, dinoSurface);
        SDL_DestroySurface(dinoSurface);
    }
    if (cactusSurface) {
        cactusTexture = SDL_CreateTextureFromSurface(renderer, cactusSurface);
        SDL_DestroySurface(cactusSurface);
    }

    // 4. Khoi tao trang thai Game
    Entity dino { 100, (float)(GROUND_Y - 50), 44, 50 }; // Khung long
    std::vector<Entity> cacti;                          //  Xương rồng

    float gameSpeed = 5.0f;
    int score = 0;
    int spawnTimer = 0;
    bool isGameOver = false;
    bool running = true;
    SDL_Event event;

    // Game loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_SPACE) {       // Space de nhay
                    if (isGameOver) {
                        // Reset khi Game Over
                        dino.y = GROUND_Y - dino.h;
                        dino.velocityY = 0;
                        cacti.clear();
                        score = 0;
                        gameSpeed = 5.0f;
                        isGameOver = false;
                    } else if (dino.isGrounded) {
                        dino.velocityY = JUMP_FORCE;
                        dino.isGrounded = false;
                    }
                }
            }
        }

        if (!isGameOver) {

            dino.y += dino.velocityY;
            dino.velocityY += GRAVITY;

            // Mat dat
            if (dino.y >= GROUND_Y - dino.h) {
                dino.y = GROUND_Y - dino.h;
                dino.velocityY = 0;
                dino.isGrounded = true;
            }

            // Spawn xuong rong
            spawnTimer++;
            if (spawnTimer > 100 + (rand() % 80)) {
                cacti.push_back({ (float)SCREEN_WIDTH, (float)(GROUND_Y - 45), 25, 45 });
                spawnTimer = 0;
            }

            // Kiem tra va cham
            for (size_t i = 0; i < cacti.size(); ) {
                cacti[i].x -= gameSpeed;

                SDL_FRect dinoBox = { dino.x + 5, dino.y + 5, (float)dino.w - 10, (float)dino.h - 5 };
                SDL_FRect cactusBox = { cacti[i].x + 3, cacti[i].y, (float)cacti[i].w - 6, (float)cacti[i].h };

                if (SDL_HasRectIntersectionFloat(&dinoBox, &cactusBox)) {
                    isGameOver = true;
                }

                // Tinh diem
                if (cacti[i].x + cacti[i].w < 0) {
                    cacti.erase(cacti.begin() + i);
                    score += 10;
                    gameSpeed += 0.1f; // Speed tang dan
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

        SDL_FRect dinoDrawRect = { dino.x, dino.y, (float)dino.w, (float)dino.h };
        if (dinoTexture) {
            SDL_RenderTexture(renderer, dinoTexture, NULL, &dinoDrawRect);
        } else {
            SDL_SetRenderDrawColor(renderer, 83, 83, 83, 255);
            SDL_RenderFillRect(renderer, &dinoDrawRect);
        }

        for (const auto& cactus : cacti) {
            SDL_FRect cactusDrawRect = { cactus.x, cactus.y, (float)cactus.w, (float)cactus.h };
            if (cactusTexture) {
                SDL_RenderTexture(renderer, cactusTexture, NULL, &cactusDrawRect);
            } else {
                SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
                SDL_RenderFillRect(renderer, &cactusDrawRect);
            }
        }

        if (isGameOver) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 50); // Màu đỏ trong suốt
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_FRect overlay = {0, 0, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
            SDL_RenderFillRect(renderer, &overlay);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    // 5. Giai phongd tai nguyen
    if (dinoTexture) SDL_DestroyTexture(dinoTexture);
    if (cactusTexture) SDL_DestroyTexture(cactusTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Tro choi ket thuc! Diem cua ban: " << score << std::endl;
    return 0;
}

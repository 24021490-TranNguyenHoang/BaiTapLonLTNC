#include "Game.h"

Game* game = nullptr;

int main(int argc, char* argv[]) {
    game = new Game();

    if (!game->init("DinoRun - OOP Architecture", 800, 400)) {
        return -1;
    }

    Uint64 lastTime = SDL_GetTicks();

    // Vòng lặp Game chạy qua class Game chuyên biệt
    while (game->isRunning()) {
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        game->handleEvents();
        game->update(deltaTime);
        game->render();

        SDL_Delay(16); // Giới hạn tương đối ~60 FPS
    }

    delete game;
    return 0;
}

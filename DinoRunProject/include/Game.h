#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include "Dino.h"
#include "Obstacle.h"

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAMEOVER
};

class Game {
public:
    Game();
    ~Game();

    bool init(const char* title, int width, int height);
    void handleEvents();
    void update(float deltaTime);
    void render();
    void clean();

    bool isRunning() const { return running; }

private:
    SDL_Texture* createTextTexture(const char* text, SDL_Color color, float& outW, float& outH);

    // Thành phần SDL cốt lõi
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;
    bool running = false;

    // Kích thước & Hằng số môi trường
    int screenWidth;
    int screenHeight;
    const int groundY = 350;

    // Tài nguyên Game (Textures)
    SDL_Texture* spearTexture = nullptr;
    int spearFrameW = 0, spearFrameH = 0;

    SDL_Texture* bgTexture = nullptr;
    float bgWidth = 0;
    float bgX = 0.0f;
    const float bgSpeedOffset = 0.5f;

    // Giao diện Menu Textures
    SDL_Texture* tPlay = nullptr;
    SDL_Texture* tRecord = nullptr;
    SDL_Texture* tQuit = nullptr;

    SDL_FRect rectPlay;
    SDL_FRect rectRecord;
    SDL_FRect rectQuit;

    SDL_Color textColor = { 83, 83, 83, 255 };
    SDL_Color hoverColor = { 255, 100, 100, 255 };

    // Thực thể & Logic Game
    Dino dino;
    std::vector<Obstacle> obstacles;
    GameState currentState = STATE_MENU;

    float gameSpeed = 5.0f;
    int score = 0;
    int highScore = 0;
    int spawnTimer = 0;
};

#endif // GAME_H_INCLUDED

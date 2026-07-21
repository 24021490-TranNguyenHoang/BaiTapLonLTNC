#include "Game.h"
#include <iostream>
#include <string>

Game::Game() {}

Game::~Game() {
    clean();
}

SDL_Texture* Game::createTextTexture(const char* text, SDL_Color color, float& outW, float& outH) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, 0, color);
    if (!surface) return nullptr;
    outW = (float)surface->w;
    outH = (float)surface->h;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return texture;
}

bool Game::init(const char* title, int width, int height) {
    screenWidth = width;
    screenHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Khong the khoi tao SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    if (!TTF_Init()) {
        std::cerr << "Khong the khoi tao TTF: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, screenWidth, screenHeight, 0);
    renderer = SDL_CreateRenderer(window, NULL);
    if (!window || !renderer) return false;

    font = TTF_OpenFont("assets/font.ttf", 28);
    if (!font) {
        std::cerr << "Loi load font chu: " << SDL_GetError() << std::endl;
        return false;
    }

    dino.init(renderer);

    if (!audio.init()) {
        std::cerr << "Loi khoi tao he thong am thanh" << std::endl;
    }
    audio.loadMenuMusic("assets/nhac_nen.mp3");
    audio.loadGameMusic("assets/nhac_ingame.mp3");
    audio.loadSfxJump("assets/jump_sound.wav");
    audio.loadSfxHit("assets/stab-sound.wav");

    SDL_Surface* spearSurface = SDL_LoadPNG("assets/spear_sheet_trimmed.png");
    if (spearSurface) {
        spearTexture = SDL_CreateTextureFromSurface(renderer, spearSurface);
        spearFrameW = spearSurface->w / SPEAR_FRAME_COUNT;
        spearFrameH = spearSurface->h;
        SDL_DestroySurface(spearSurface);
    }

    const char* shurikenPaths[SHURIKEN_FRAME_COUNT] = {
        "assets/shuriken3a.png",
        "assets/shuriken3b.png",
        "assets/shuriken3c.png",
        "assets/shuriken3d.png"
    };

    for (int i = 0; i < SHURIKEN_FRAME_COUNT; i++) {
        SDL_Surface* surf = SDL_LoadPNG(shurikenPaths[i]);
        if (!surf) {
            std::cerr << "Khong load duoc " << shurikenPaths[i] << ": " << SDL_GetError() << std::endl;
            continue;
        }
        shurikenFrames[i] = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_DestroySurface(surf);
    }

    SDL_Surface* bgSurface = SDL_LoadPNG("assets/background.png");
    if (bgSurface) {
        bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
        bgWidth = (float)bgSurface->w;
        SDL_DestroySurface(bgSurface);
    } else {
        std::cerr << "Loi load background: " << SDL_GetError() << std::endl;
    }

    // Tao Menu Textures
    float wPlay, hPlay, wRecord, hRecord, wQuit, hQuit;
    tPlay = createTextTexture("PLAY", textColor, wPlay, hPlay);
    tRecord = createTextTexture("HIGH SCORE", textColor, wRecord, hRecord);
    tQuit = createTextTexture("QUIT", textColor, wQuit, hQuit);

    rectPlay   = { (screenWidth - wPlay) / 2, 90, wPlay, hPlay };
    rectRecord = { (screenWidth - wRecord) / 2, 140, wRecord, hRecord };
    rectQuit   = { (screenWidth - wQuit) / 2, 190, wQuit, hQuit };

    running = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    float mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == SDLK_SPACE && currentState == STATE_PLAYING) {
                dino.jump();
                audio.playJump();
            }
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            if (currentState == STATE_MENU) {
                if (mouseX >= rectPlay.x && mouseX <= rectPlay.x + rectPlay.w &&
                    mouseY >= rectPlay.y && mouseY <= rectPlay.y + rectPlay.h) {
                    currentState = STATE_PLAYING;
                }
                else if (mouseX >= rectRecord.x && mouseX <= rectRecord.x + rectRecord.w &&
                         mouseY >= rectRecord.y && mouseY <= rectRecord.y + rectRecord.h) {
                    std::cout << "Current High Score: " << highScore << std::endl;
                }
                else if (mouseX >= rectQuit.x && mouseX <= rectQuit.x + rectQuit.w &&
                         mouseY >= rectQuit.y && mouseY <= rectQuit.y + rectQuit.h) {
                    running = false;
                }
            }
            else if (currentState == STATE_GAMEOVER) {
                dino.reset(groundY);
                obstacles.clear();
                shurikens.clear();
                score = 0;
                gameSpeed = 5.0f;
                lastSpeedUpScore = 0;
                currentState = STATE_MENU;
            }
        }
    }
}

void Game::update(float deltaTime) {
     if (currentState == STATE_MENU) {
        audio.playMenuMusic();
    }
    else if (currentState == STATE_PLAYING) {
        audio.playGameMusic();
    }
    if (currentState == STATE_PLAYING) {
        bgX -= gameSpeed * bgSpeedOffset;
        if (bgX <= -bgWidth) {
            bgX = 0;
        }

        dino.update(groundY, deltaTime);

        spawnTimer++;
        if (spawnTimer > 100 + (rand() % 80)) {
            Obstacle trap;
            trap.init(spearTexture, spearFrameW, spearFrameH);
            trap.x = (float)screenWidth;
            trap.y = groundY - trap.h;
            obstacles.push_back(trap);
            spawnTimer = 0;
        }

        for (size_t i = 0; i < obstacles.size(); ) {
            obstacles[i].update(deltaTime, gameSpeed);

            SDL_FRect dinoBox = { dino.getX() + 3, dino.getY() + 3, (float)dino.getW() - 6, (float)dino.getH() - 6 };
            SDL_FRect trapBox = obstacles[i].getHitbox();

            if (SDL_HasRectIntersectionFloat(&dinoBox, &trapBox)) {
                currentState = STATE_GAMEOVER;
                if (score > highScore) highScore = score;
                audio.playHit();
                audio.stopAllMusic();
            }

            if (obstacles[i].isOffScreen()) {
                obstacles.erase(obstacles.begin() + i);
                score += 10;
                gameSpeed += 0.1f;
                if (score - lastSpeedUpScore >= 100) {
                    gameSpeed += 1.0f;
                    lastSpeedUpScore = score - (score % 100);
                }
            } else {
                i++;
            }
        }

        // Spawn shuriken bay ngang, do cao luon co dinh
        shurikenSpawnTimer++;
        if (shurikenSpawnTimer > 200 + (rand() % 150)) {
            Shuriken shuriken;
            shuriken.init(shurikenFrames);
            shuriken.x = (float)screenWidth;
            shuriken.y = (float)shurikenFixedY;
            shurikens.push_back(shuriken);
            shurikenSpawnTimer = 0;
        }

        for (size_t i = 0; i < shurikens.size(); ) {
            shurikens[i].update(deltaTime, gameSpeed);

            SDL_FRect dinoBox = { dino.getX() + 3, dino.getY() + 3, (float)dino.getW() - 6, (float)dino.getH() - 6 };
            SDL_FRect shurikenBox = shurikens[i].getHitbox();

            if (SDL_HasRectIntersectionFloat(&dinoBox, &shurikenBox)) {
                currentState = STATE_GAMEOVER;
                if (score > highScore) highScore = score;
                audio.playHit();
                audio.stopAllMusic();
            }

            if (shurikens[i].isOffScreen()) {
                shurikens.erase(shurikens.begin() + i);
                score += 15;
            } else {
                i++;
            }
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 247, 247, 247, 255);
    SDL_RenderClear(renderer);

    // 1. Ve hinh nen duoi cung
    if (bgTexture) {
        SDL_FRect bgRect1 = { bgX, 0, bgWidth, (float)screenHeight };
        SDL_RenderTexture(renderer, bgTexture, NULL, &bgRect1);

        SDL_FRect bgRect2 = { bgX + bgWidth, 0, bgWidth, (float)screenHeight };
        SDL_RenderTexture(renderer, bgTexture, NULL, &bgRect2);
    }

    // 2. Chuyen doi trang thai man hinh
    if (currentState == STATE_MENU) {
        float mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Hieu ung Hover nut
        auto checkHover = [&](SDL_FRect& r, SDL_Texture* t) {
            if (mouseX >= r.x && mouseX <= r.x + r.w && mouseY >= r.y && mouseY <= r.y + r.h)
                SDL_SetTextureColorMod(t, hoverColor.r, hoverColor.g, hoverColor.b);
            else
                SDL_SetTextureColorMod(t, textColor.r, textColor.g, textColor.b);
        };

        checkHover(rectPlay, tPlay);
        checkHover(rectRecord, tRecord);
        checkHover(rectQuit, tQuit);

        SDL_RenderTexture(renderer, tPlay, NULL, &rectPlay);
        SDL_RenderTexture(renderer, tRecord, NULL, &rectRecord);
        SDL_RenderTexture(renderer, tQuit, NULL, &rectQuit);
    }
    else if (currentState == STATE_PLAYING || currentState == STATE_GAMEOVER) {
        dino.render(renderer);
        for (auto& obs : obstacles) {
            obs.render(renderer);
        }
        for (auto& shuriken : shurikens) {
            shuriken.render(renderer);
        }

        if (currentState == STATE_PLAYING) {
            float wS, hS;
            std::string scoreStr = "SCORE: " + std::to_string(score);
            SDL_Texture* tScore = createTextTexture(scoreStr.c_str(), textColor, wS, hS);
            SDL_FRect rectScore = { screenWidth - wS - 20, 20, wS, hS };
            SDL_RenderTexture(renderer, tScore, NULL, &rectScore);
            SDL_DestroyTexture(tScore);
        }
        else { // STATE_GAMEOVER
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 40);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_FRect overlay = {0, 0, (float)screenWidth, (float)screenHeight};
            SDL_RenderFillRect(renderer, &overlay);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

            float wGo, hGo;
            SDL_Texture* tGo = createTextTexture("GAME OVER - CLICK TO RETURN MENU", hoverColor, wGo, hGo);
            SDL_FRect rectGo = { (screenWidth - wGo) / 2, 150, wGo, hGo };
            SDL_RenderTexture(renderer, tGo, NULL, &rectGo);
            SDL_DestroyTexture(tGo);
        }
    }

    SDL_RenderPresent(renderer);
}

void Game::clean() {
    SDL_DestroyTexture(tPlay);
    SDL_DestroyTexture(tRecord);
    SDL_DestroyTexture(tQuit);
    if (spearTexture) SDL_DestroyTexture(spearTexture);
    if (bgTexture) SDL_DestroyTexture(bgTexture);
    for (int i = 0; i < SHURIKEN_FRAME_COUNT; i++) {
        if (shurikenFrames[i]) SDL_DestroyTexture(shurikenFrames[i]);
    }
    if (font) TTF_CloseFont(font);
    audio.clean();

    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

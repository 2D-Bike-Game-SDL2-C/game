#include "game.h"
#include "constants.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>

Game::Game() : 
    window(nullptr), 
    renderer(nullptr), 
    gameState(GameState::PLAYING),
    menuState(MenuState::MAIN_MENU),
    running(false) {
}

Game::~Game() {
    clean();
}

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!TheTextureManager::Instance()->init()) {
        std::cerr << "TextureManager initialization failed!" << std::endl;
        return false;
    }

    if (!TheTextureManager::Instance()->loadTexture(PLAYER_TEXTURE_PATH, PLAYER_TEXTURE_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture(OBSTACLE_TEXTURE_PATH, OBSTACLE_TEXTURE_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture(COIN_TEXTURE_PATH, COIN_TEXTURE_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture(COIN_FINISH_PATH, FINISH_TEXTURE_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture(COIN_BACKGROUND_PATH, BACKGROUND_TEXTURE_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture(COIN_BACKGROUND_PATH, MENU_BACKGROUND_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture("assets/about.png", ABOUT_BACKGROUND_ID, renderer)) {
        std::cerr << "Failed to load textures!" << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return false;
    }
    font = TTF_OpenFont("assets/arial.ttf", 24);
    if (!font) {
        std::cerr << "Font Load Error: " << TTF_GetError() << std::endl;
        return false;
    }

    player = std::make_unique<Player>(SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2, SCREEN_HEIGHT - PLAYER_HEIGHT - 50);
    player->setTextureID(PLAYER_TEXTURE_ID);
    gameMap = std::make_unique<GameMap>();

    initMenus();

    running = true;
    return true;
}

void Game::initMenus() {
    mainMenu = std::make_unique<MainMenu>(this);
    mainMenu->setBackground(MENU_BACKGROUND_ID);
    
    pauseMenu = std::make_unique<PauseMenu>(this);
    pauseMenu->setBackground(MENU_BACKGROUND_ID);
    
    gameOverMenu = std::make_unique<GameOverMenu>(this);
    gameOverMenu->setBackground(MENU_BACKGROUND_ID);
    
    optionsMenu = std::make_unique<OptionsMenu>(this);
    optionsMenu->setBackground(MENU_BACKGROUND_ID);
    
    aboutMenu = std::make_unique<AboutMenu>(this);
    aboutMenu->setBackground(ABOUT_BACKGROUND_ID);
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            return;
        }

        switch (menuState) {
            case MenuState::MAIN_MENU:
                if (mainMenu->handleEvent(event)) {
                    continue;
                }
                break;
            case MenuState::PAUSE_MENU:
                if (pauseMenu->handleEvent(event)) {
                    continue;
                }
                break;
            case MenuState::GAME_OVER:
                if (gameOverMenu->handleEvent(event)) {
                    continue;
                }
                break;
            case MenuState::OPTIONS_MENU:
                if (optionsMenu->handleEvent(event)) {
                    continue;
                }
                break;
            case MenuState::ABOUT:
                if (aboutMenu->handleEvent(event)) {
                    continue;
                }
                break;
            case MenuState::LEVEL_COMPLETE:
            case MenuState::GAME_PLAYING:
                break;
        }

        if (menuState == MenuState::GAME_PLAYING && gameState == GameState::PLAYING) {
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDL_SCANCODE_ESCAPE) {
                    setGameState(MenuState::PAUSE_MENU);
                }
            }
        }
    }

    if (menuState == MenuState::GAME_PLAYING && gameState == GameState::PLAYING) {
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) {
            player->moveLeft();
        }
        if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) {
            player->moveRight();
        }
    }
}

void Game::update() {
    switch (menuState) {
        case MenuState::MAIN_MENU:
            mainMenu->update();
            break;
        case MenuState::PAUSE_MENU:
            pauseMenu->update();
            break;
        case MenuState::GAME_OVER:
            gameOverMenu->update();
            break;
        case MenuState::OPTIONS_MENU:
            optionsMenu->update();
            break;
        case MenuState::ABOUT:
            aboutMenu->update();
            break;
        case MenuState::LEVEL_COMPLETE:
        case MenuState::GAME_PLAYING:
            if (gameState == GameState::PLAYING) {
                player->update();
                gameMap->update();
                
                int points = 0;
                bool collision = gameMap->checkCollision(player->getRect(), points);
                
                if (points > 0) {
                    player->addScore(points);
                    if (points >= MAX_SCORE) {
                        gameState = GameState::FINISHED;
                        menuState = MenuState::LEVEL_COMPLETE;
                        gameOverMenu->setResults(player->getScore(), gameMap->getScrolledRows());
                    }
                }
                
                if (collision) {
                    player->kill();
                    gameState = GameState::GAME_OVER;
                    menuState = MenuState::GAME_OVER;
                    gameOverMenu->setResults(player->getScore(), gameMap->getScrolledRows());
                }
            }
            break;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    switch (menuState) {
        case MenuState::MAIN_MENU:
            mainMenu->render(renderer);
            break;
        case MenuState::PAUSE_MENU:
            TheTextureManager::Instance()->draw(BACKGROUND_TEXTURE_ID, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
            gameMap->render(renderer);
            player->render(renderer);
            renderUI();
            pauseMenu->render(renderer);
            break;
        case MenuState::GAME_OVER:
            TheTextureManager::Instance()->draw(BACKGROUND_TEXTURE_ID, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
            gameMap->render(renderer);
            player->render(renderer);
            renderUI();
            gameOverMenu->render(renderer);
            break;
        case MenuState::OPTIONS_MENU:
            optionsMenu->render(renderer);
            break;
        case MenuState::ABOUT:
            aboutMenu->render(renderer);
            break;
        case MenuState::LEVEL_COMPLETE:
        case MenuState::GAME_PLAYING:
            TheTextureManager::Instance()->draw(BACKGROUND_TEXTURE_ID, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
            gameMap->render(renderer);
            player->render(renderer);
            renderUI();
            
            if (menuState == MenuState::LEVEL_COMPLETE) {
                SDL_Color messageColor = {0, 255, 0, 255};
                const char* message = "Level Complete! Press R to restart";
                
                TTF_Font* bigFont = TTF_OpenFont("assets/arial.ttf", 36);
                if (bigFont) {
                    SDL_Surface* msgSurface = TTF_RenderText_Blended(bigFont, message, messageColor);
                    if (msgSurface) {
                        SDL_Texture* msgTexture = SDL_CreateTextureFromSurface(renderer, msgSurface);
                        SDL_Rect msgRect = {
                            (SCREEN_WIDTH - msgSurface->w) / 2,
                            (SCREEN_HEIGHT - msgSurface->h) / 2,
                            msgSurface->w,
                            msgSurface->h
                        };
                        SDL_RenderCopy(renderer, msgTexture, nullptr, &msgRect);
                        
                        SDL_FreeSurface(msgSurface);
                        SDL_DestroyTexture(msgTexture);
                    }
                    TTF_CloseFont(bigFont);
                }
            }
            break;
    }

    SDL_RenderPresent(renderer);
}

void Game::renderUI() {
    if (!font) return;
    
    if (menuState == MenuState::GAME_PLAYING || menuState == MenuState::PAUSE_MENU || menuState == MenuState::GAME_OVER) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
        SDL_Rect scoreBg = {10, 10, 150, 40};
        SDL_RenderFillRect(renderer, &scoreBg);
        
        SDL_Rect distanceBg = {SCREEN_WIDTH - 160, 10, 150, 40};
        SDL_RenderFillRect(renderer, &distanceBg);

        SDL_Color textColor = {255, 255, 255, 255};
        std::string scoreText = "Score: " + std::to_string(player->getScore());
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {20, 15, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
        
        std::string distanceText = "Distance: " + std::to_string(gameMap->getScrolledRows());
        textSurface = TTF_RenderText_Solid(font, distanceText.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {SCREEN_WIDTH - 150, 15, textSurface->w, textSurface->h};
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
    }
}

void Game::clean() {
    TheTextureManager::Instance()->clean();
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
}

void Game::run() {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;
    Uint32 frameStart;
    int frameTime;

    while (running) {
        frameStart = SDL_GetTicks();
        
        handleEvents();
        update();
        render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Game::setGameState(MenuState state) {
    menuState = state;
    
    switch (menuState) {
        case MenuState::GAME_PLAYING:
            gameState = GameState::PLAYING;
            break;
        case MenuState::GAME_OVER:
            gameState = GameState::GAME_OVER;
            break;
        case MenuState::LEVEL_COMPLETE:
            gameState = GameState::FINISHED;
            break;
        case MenuState::MAIN_MENU:
        case MenuState::PAUSE_MENU:
        case MenuState::OPTIONS_MENU:
        case MenuState::ABOUT:
            // No change to gameState
            break;
    }
}

void Game::quit() {
    running = false;
}

void Game::restart() {
    player = std::make_unique<Player>(SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2, SCREEN_HEIGHT - PLAYER_HEIGHT - 50);
    player->setTextureID(PLAYER_TEXTURE_ID);
    gameMap = std::make_unique<GameMap>();
    gameState = GameState::PLAYING;
    menuState = MenuState::GAME_PLAYING;
}
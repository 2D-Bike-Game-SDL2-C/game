#include "game.h"
#include "constants.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>

Game::Game() : 
    window(nullptr), 
    renderer(nullptr), 
    gameState(GameState::PLAYING),
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

    // Initialize texture manager
    if (!TheTextureManager::Instance()->init()) {
        std::cerr << "TextureManager initialization failed!" << std::endl;
        return false;
    }

    // Load textures
    if (!TheTextureManager::Instance()->loadTexture(PLAYER_TEXTURE_PATH, PLAYER_TEXTURE_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture(OBSTACLE_TEXTURE_PATH, OBSTACLE_TEXTURE_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture(COIN_TEXTURE_PATH, COIN_TEXTURE_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture(COIN_FINISH_PATH, FINISH_TEXTURE_ID, renderer) ||
        !TheTextureManager::Instance()->loadTexture(COIN_BACKGROUND_PATH, BACKGROUND_TEXTURE_ID, renderer))
        {
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

    // Initialize game objects
    player = std::make_unique<Player>(SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2, SCREEN_HEIGHT - PLAYER_HEIGHT - 50);
    player->setTextureID(PLAYER_TEXTURE_ID);
    gameMap = std::make_unique<GameMap>();

    running = true;
    return true;
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }

    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    if (gameState == GameState::PLAYING) {
        if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) {
            player->moveLeft();
        }
        if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) {
            player->moveRight();
        }
    }
    
    if (currentKeyStates[SDL_SCANCODE_R] && gameState != GameState::PLAYING) {
        player = std::make_unique<Player>(SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2, SCREEN_HEIGHT - PLAYER_HEIGHT - 50);
        player->setTextureID(PLAYER_TEXTURE_ID);
        gameMap = std::make_unique<GameMap>();
        gameState = GameState::PLAYING;
    }
    if (currentKeyStates[SDL_SCANCODE_ESCAPE]) {
        running = false;
    }
}

void Game::update() {
    if (gameState == GameState::PLAYING) {
        player->update();
        gameMap->update();
        
        int points = 0;
        bool collision = gameMap->checkCollision(player->getRect(), points);
        
        if (points > 0) {
            player->addScore(points);
            if (points >= MAX_SCORE) {
                gameState = GameState::FINISHED;
            }
        }
        
        if (collision) {
            player->kill();
            gameState = GameState::GAME_OVER;
        }
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderClear(renderer);

    // Draw background
    TheTextureManager::Instance()->draw(BACKGROUND_TEXTURE_ID, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, renderer);

    gameMap->render(renderer);
    player->render(renderer);
    
    renderUI();
    
    SDL_RenderPresent(renderer);
}

void Game::renderUI() {
    if (!font) return;
    
    // Score and distance backgrounds
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_Rect scoreBg = {10, 10, 150, 40};
    SDL_RenderFillRect(renderer, &scoreBg);
    
    SDL_Rect distanceBg = {SCREEN_WIDTH - 160, 10, 150, 40};
    SDL_RenderFillRect(renderer, &distanceBg);

    // Render score text
    SDL_Color textColor = {255, 255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(player->getScore());
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {20, 15, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    
    // Render distance text
    std::string distanceText = "Distance: " + std::to_string(gameMap->getScrolledRows());
    textSurface = TTF_RenderText_Solid(font, distanceText.c_str(), textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    textRect = {SCREEN_WIDTH - 150, 15, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    // Cleanup
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    

    // Game state messages
    if (gameState != GameState::PLAYING) {
        SDL_Color messageColor = gameState == GameState::FINISHED ? 
            SDL_Color{0, 255, 0, 255} : SDL_Color{255, 0, 0, 255};
        
        const char* message = gameState == GameState::FINISHED ? 
            "Level Complete! Press R to restart" : "Game Over! Press R to restart";
        
        TTF_Font* bigFont = TTF_OpenFont("assets/arial.ttf", 36);
        if (!bigFont) return;

        SDL_Surface* msgSurface = TTF_RenderText_Blended(bigFont, message, messageColor);
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
        TTF_CloseFont(bigFont);
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
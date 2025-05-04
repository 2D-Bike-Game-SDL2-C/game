#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include "gameobject.h"
#include "gamemap.h"
#include "texturemanager.h"

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<Player> player;
    std::unique_ptr<GameMap> gameMap;
    GameState gameState;
    TTF_Font* font = nullptr;
    bool running;
    
    // Texture IDs
    const std::string PLAYER_TEXTURE_ID = "player";
    const std::string OBSTACLE_TEXTURE_ID = "obstacle";
    const std::string COIN_TEXTURE_ID = "coin";
    const std::string FINISH_TEXTURE_ID = "finish";
    const std::string BACKGROUND_TEXTURE_ID = "background";

    void handleEvents();
    void update();
    void render();
    void renderUI();
    void clean();

public:
    Game();
    ~Game();
    bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void run();
};

#endif // GAME_H
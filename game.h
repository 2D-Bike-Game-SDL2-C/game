#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include "gameobject.h"
#include "gamemap.h"
#include "texturemanager.h"
#include "menu.h"

class Game {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::unique_ptr<Player> player;
    std::unique_ptr<GameMap> gameMap;
    GameState gameState;
    MenuState menuState;
    TTF_Font* font = nullptr;
    bool running;

    // Menus
    std::unique_ptr<MainMenu> mainMenu;
    std::unique_ptr<PauseMenu> pauseMenu;
    std::unique_ptr<GameOverMenu> gameOverMenu;
    std::unique_ptr<OptionsMenu> optionsMenu;
    std::unique_ptr<AboutMenu> aboutMenu;  // Added for About screen
    
    // Texture IDs
    const std::string PLAYER_TEXTURE_ID = "player";
    const std::string OBSTACLE_TEXTURE_ID = "obstacle";
    const std::string COIN_TEXTURE_ID = "coin";
    const std::string FINISH_TEXTURE_ID = "finish";
    const std::string BACKGROUND_TEXTURE_ID = "background";
    const std::string MENU_BACKGROUND_ID = "menu_background";
    const std::string ABOUT_BACKGROUND_ID = "about_background";  // Added for About screen

    void handleEvents();
    void update();
    void render();
    void renderUI();
    void clean();
    void initMenus(); // Initialize menus

public:
    Game();
    ~Game();
    bool init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
    void run();

    // Menu control methods
    void setGameState(MenuState state);
    void quit();
    void restart();
    
};

#endif // GAME_H
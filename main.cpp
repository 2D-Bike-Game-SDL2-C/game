//main.cpp
#include "game.h"
#include "constants.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Game game;
    
    if (!game.init("Endless Runner", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                  SCREEN_WIDTH, SCREEN_HEIGHT, false)) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return 1;
    }
    
    game.run();
    
    return 0;
}
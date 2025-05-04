#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

// Game constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 40; // Size of each grid cell in pixels
const int GRID_COLS = SCREEN_WIDTH / GRID_SIZE;
const int GRID_ROWS = SCREEN_HEIGHT / GRID_SIZE + 1; // +1 for the row being scrolled in
const int BUFFER_ROWS = 10; // Number of rows to pre-generate above screen
const int PLAYER_WIDTH = 30;
const int PLAYER_HEIGHT = 50;
const float SCROLL_SPEED = 3.0f; // Pixels per frame
const float PLAYER_SPEED = 5.0f; // Pixels per frame

const std::string PLAYER_TEXTURE_PATH = "assets/player.png";
const std::string OBSTACLE_TEXTURE_PATH = "assets/obstacle.png";
const std::string COIN_TEXTURE_PATH = "assets/coin.png";

// Cell types
enum class CellType {
    EMPTY,
    OBSTACLE,
    COIN,
    FINISH
};

// Game state
enum class GameState {
    PLAYING,
    GAME_OVER,
    FINISHED
};

#endif // CONSTANTS_H
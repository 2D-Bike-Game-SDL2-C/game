#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

// Game constants
const int SCREEN_WIDTH = 1280;   //800
const int SCREEN_HEIGHT = 960;  //600
const int GRID_SIZE = 60; // 40 Size of each grid cell in pixels
const int GRID_COLS = SCREEN_WIDTH / GRID_SIZE;
const int GRID_ROWS = SCREEN_HEIGHT / GRID_SIZE + 1; // +1 for the row being scrolled in
const int BUFFER_ROWS = 10; // Number of rows to pre-generate above screen
const int PLAYER_WIDTH = 50;
const int PLAYER_HEIGHT = 80;
const float SCROLL_SPEED = 3.0f; // Pixels per frame
const float PLAYER_SPEED = 5.0f; // Pixels per frame
const int MAX_SCORE = 1000;
const int MAX_ROWS = 200;

const std::string PLAYER_TEXTURE_PATH = "assets/player2.png";
const std::string OBSTACLE_TEXTURE_PATH = "assets/obstacle22.png";
const std::string COIN_TEXTURE_PATH = "assets/coin6.png";
const std::string COIN_BACKGROUND_PATH = "assets/background3.png";
const std::string COIN_FINISH_PATH = "assets/finish.png";

const int PLAYER_FRAMES = 6;
const int COIN_FRAMES = 9;

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

#endif
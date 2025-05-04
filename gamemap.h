#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>
#include <random>
#include <memory>
#include "gameobject.h"
#include "texturemanager.h"

class GameMap {
private:
    std::vector<std::vector<std::unique_ptr<Cell>>> grid;
    int totalRows;
    float scrollOffset;  // Fraction of a grid cell (0.0 to GRID_SIZE)
    int scrolledRows;    // Track total rows scrolled for level progression
    std::mt19937 rng;
    int difficultyLevel;
    bool finishLineGenerated;
    
    // Texture IDs for different cell types
    const std::string OBSTACLE_TEXTURE_ID = "obstacle";
    const std::string COIN_TEXTURE_ID = "coin";
    const std::string FINISH_TEXTURE_ID = "finish";

    void generateRow(int rowIndex);
    void shiftRowsDown();

public:
    GameMap();
    void update();
    void render(SDL_Renderer* renderer) const;
    bool checkCollision(const SDL_Rect& playerRect, int& points);
    int getScrolledRows() const;
};

#endif // GAMEMAP_H
#include "gamemap.h"
#include <cmath> // For fmod

GameMap::GameMap() : 
    totalRows(GRID_ROWS + BUFFER_ROWS),
    scrollOffset(0.0f),
    scrolledRows(0),
    difficultyLevel(1),
    finishLineGenerated(false) {
    
    // Initialize random number generator
    std::random_device rd;
    rng = std::mt19937(rd());
    
    // Initialize grid with empty cells
    grid.resize(totalRows);
    for (auto& row : grid) {
        row.resize(GRID_COLS);
        for (int col = 0; col < GRID_COLS; col++) {
            row[col] = nullptr;
        }
    }
    
    // Generate initial map
    for (int i = 0; i < totalRows; ++i) {
        generateRow(i);
    }
}

void GameMap::update() {
    // Update scroll offset
    scrollOffset += SCROLL_SPEED;
    
    // Check if we need to shift rows
    if (scrollOffset >= GRID_SIZE) {
        // Calculate how many rows to shift
        int rowsToShift = static_cast<int>(scrollOffset) / GRID_SIZE;
        scrollOffset = fmod(scrollOffset, static_cast<float>(GRID_SIZE));
        
        // Shift rows
        for (int i = 0; i < rowsToShift; i++) {
            shiftRowsDown();
            scrolledRows++;
        }
        
        // Increase difficulty periodically
        /*if (scrolledRows % 50 == 0 && difficultyLevel < 10) {
            difficultyLevel++;
        }*/
    }
    
    // Update all cells in the grid
    for (auto& row : grid) {
        for (auto& cell : row) {
            if (cell) {
                cell->update();             //until now there nothing to update
            }
        }
    }
}

void GameMap::render(SDL_Renderer* renderer) const {
    // Convert scrollOffset to integer for rendering
    int intScrollOffset = static_cast<int>(scrollOffset);

    // Draw grid cells
    for (int row = 0; row < totalRows; ++row) {
        // Calculate screen Y position
        int screenY = row * GRID_SIZE + intScrollOffset;
        
        // Only draw rows that are visible
        if (screenY < SCREEN_HEIGHT) {                         //screenY + GRID_SIZE > 0 &&
            for (int col = 0; col < GRID_COLS; ++col) {
                if (grid[row][col]) {
                    // Calculate screen position
                    SDL_Rect tempRect = {
                        col * GRID_SIZE, //x
                        screenY,         //y
                        GRID_SIZE,       //w
                        GRID_SIZE        //h
                    };
                    
                    Cell* cell = dynamic_cast<Cell*>(grid[row][col].get());
                    if (cell && !cell->isCollected() && !cell->getTextureID().empty()) {
                        cell->render(renderer, tempRect);
                    }
                    /*else {
                        // Fallback to default rendering (optional, but provides safety)
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        SDL_RenderDrawRect(renderer, &tempRect);
                    }*/
                    }
                }
            }
        }
}

void GameMap::shiftRowsDown() {
    // Remove bottom row
    grid.pop_back();
    
    // Add new row at top
    std::vector<std::unique_ptr<Cell>> newRow(GRID_COLS);
    for (int i = 0; i < GRID_COLS; i++) {
        newRow[i] = nullptr;
    }
    grid.insert(grid.begin(), std::move(newRow));
    
    // Generate content for new row
    generateRow(0);
}

void GameMap::generateRow(int rowIndex) {

    // Calculate if this should be the finish line
    bool isFinishLine = !finishLineGenerated && scrolledRows > MAX_ROWS && rowIndex == 0;
    
    if (isFinishLine) {
        for (int col = 0; col < GRID_COLS; ++col) {
            grid[rowIndex][col] = std::make_unique<Cell>(
                CellType::FINISH, 
                col * GRID_SIZE, 
                rowIndex * GRID_SIZE, 
                GRID_SIZE, 
                GRID_SIZE
            );
        }
        finishLineGenerated = true;
        return;
    }
    
    // Previous row analysis for path continuity
    std::vector<bool> previousObstacles(GRID_COLS, false);
    if (rowIndex < totalRows - 1) {
        for (int col = 0; col < GRID_COLS; ++col) {
            if (grid[rowIndex + 1][col] && dynamic_cast<Cell*>(grid[rowIndex + 1][col].get())->getType() == CellType::OBSTACLE) {
                previousObstacles[col] = true;
            }
        }
    }
    
    // Generate new row with procedural content
    std::uniform_int_distribution<int> obstacleChance(1, 100);
    std::uniform_int_distribution<int> coinChance(1, 100);
    
    // Make sure there's at least one safe path
    int guaranteedSafePath = std::uniform_int_distribution<int>(0, GRID_COLS - 1)(rng);
    
    for (int col = 0; col < GRID_COLS; ++col) {
        // Initialize as empty
        CellType cellType = CellType::EMPTY;
        
        // Make procedural decisions
        if (col != guaranteedSafePath) {
            // Obstacle probability increases with difficulty
            int obstacleThreshold = 95 - difficultyLevel * 3;
            // Reduce obstacles if there was one in the previous row at this position (avoid walls)
            if (previousObstacles[col]) {
                obstacleThreshold += 10;
            }
            
            // Place obstacles
            if (obstacleChance(rng) > obstacleThreshold) {
                cellType = CellType::OBSTACLE;
            }
            // Place coins (but not where there are obstacles)
            else if (coinChance(rng) > 85) {
                cellType = CellType::COIN;
            }
        }
        
        // Create the game object
        if (cellType != CellType::EMPTY) {
            grid[rowIndex][col] = std::make_unique<Cell>(
                cellType, 
                col * GRID_SIZE, 
                rowIndex * GRID_SIZE, 
                GRID_SIZE, 
                GRID_SIZE
            );
        }
    }
}

bool GameMap::checkCollision(const SDL_Rect& playerRect, int& points) {
    // Calculate grid coordinates from player position
    int screenOffset = static_cast<int>(scrollOffset);
    
    // These are approximate grid cell coordinates

    int startRow = std::max(0, (playerRect.y - screenOffset) / GRID_SIZE);
    int endRow = std::min(totalRows - 1, (playerRect.y + playerRect.h - screenOffset) / GRID_SIZE);
    int startCol = std::max(0, playerRect.x / GRID_SIZE);
    int endCol = std::min(GRID_COLS - 1, (playerRect.x + playerRect.w) / GRID_SIZE);
    
    bool hitObstacle = false;
    
    // Check all potentially overlapping cells
    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            if (grid[row][col] && grid[row][col]->isActive() && 
                !dynamic_cast<Cell*>(grid[row][col].get())->isCollected()) {
                // Calculate screen position of this cell
                SDL_Rect cellRect = {
                    col * GRID_SIZE,
                    row * GRID_SIZE + screenOffset,
                    GRID_SIZE,
                    GRID_SIZE
                };
                
                // Check precise collision
                if (SDL_HasIntersection(&playerRect, &cellRect)) {
                    CellType type = dynamic_cast<Cell*>(grid[row][col].get())->getType();
                    
                    if (type == CellType::OBSTACLE) {
                        hitObstacle = true;
                    } else if (type == CellType::COIN) {
                        dynamic_cast<Cell*>(grid[row][col].get())->collect();
                        points += 10;
                    } else if (type == CellType::FINISH) {
                        points += 1000; // Bonus for finishing
                        return false;  // Successfully reaching the finish line isn't a collision
                    }
                }
            }
        }
    }
    
    return hitObstacle;
}

int GameMap::getScrolledRows() const {
    return scrolledRows;
}
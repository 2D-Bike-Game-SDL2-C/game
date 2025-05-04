#include "gameobject.h"

// GameObject implementation
GameObject::GameObject(int x, int y, int w, int h) : active(true) {
    rect = {x, y, w, h};
}

const SDL_Rect& GameObject::getRect() const {
    return rect;
}

bool GameObject::isActive() const {
    return active;
}

void GameObject::setActive(bool isActive) {
    active = isActive;
}

bool GameObject::checkCollision(const SDL_Rect& other) const {
    return SDL_HasIntersection(&rect, &other);
}

// Cell implementation
Cell::Cell(CellType t, int x, int y, int w, int h)
    : GameObject(x, y, w, h), type(t), collected(false) {
    // Set appropriate texture ID based on cell type
    switch (type) {
        case CellType::OBSTACLE:
            textureID = "obstacle";
            break;
        case CellType::COIN:
            textureID = "coin";
            break;
        case CellType::FINISH:
            textureID = "finish";
            break;
        default:
            textureID = "";
            break;
    }
}

void Cell::update() {
    
}

void Cell::render(SDL_Renderer* renderer) const {
    if (!active || collected) return;
    
    // Only render if we have a texture ID
    if (!textureID.empty()) {
        TheTextureManager::Instance()->draw(textureID, rect.x, rect.y, rect.w, rect.h, renderer);
    } else {
        // Fallback to original rendering if no texture is available
        switch (type) {
            case CellType::OBSTACLE:
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for obstacles
                SDL_RenderFillRect(renderer, &rect);
                break;
            case CellType::COIN:
                {  // Scope for variable declarations in switch case
                    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold for coins
                    // Draw a circle-like shape for coins
                    int centerX = rect.x + rect.w / 2;
                    int centerY = rect.y + rect.h / 2;
                    int radius = rect.w / 3;
                    for (int w = 0; w < radius * 2; w++) {
                        for (int h = 0; h < radius * 2; h++) {
                            int dx = radius - w;
                            int dy = radius - h;
                            if ((dx*dx + dy*dy) <= radius*radius) {
                                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                            }
                        }
                    }
                }
                break;
            case CellType::FINISH:
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for finish
                SDL_RenderFillRect(renderer, &rect);
                break;
            default:
                break;
        }
    }
}

CellType Cell::getType() const {
    return type;
}

bool Cell::isCollected() const {
    return collected;
}

void Cell::collect() {
    collected = true;
}

void Cell::setTextureID(const std::string& id) {
    textureID = id;
}

std::string& Cell::getTextureID()
{
    return textureID;
}

// Player implementation
Player::Player(int x, int y)
    : GameObject(x, y, PLAYER_WIDTH, PLAYER_HEIGHT), 
      score(0), 
      alive(true), 
      textureID("player"),
      currentFrame(0),
      frameCount(3),        // frames per animation
      animationSpeed(20),    // Update frame every 5 game frames
      frameCounter(0)
{}

void Player::update() {
    // Update animation frame
    frameCounter++;
    if (frameCounter > animationSpeed) {
        frameCounter = 0;
        
        // Always animate (player is always moving)
        currentFrame = (currentFrame + 1) % frameCount;
    }
}

void Player::render(SDL_Renderer* renderer) const {
    if (!active) return;
    
    if (!textureID.empty()) {
        // Use sprite sheet rendering
        TheTextureManager::Instance()->drawFrame(
            textureID, 
            rect.x, rect.y, 
            rect.w, rect.h, 
            0,                  // row 0
            currentFrame, 
            renderer
        );
    } else {
        // Fallback to original rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue for player
        SDL_RenderFillRect(renderer, &rect);
    }
}


void Player::moveLeft() {
    rect.x -= PLAYER_SPEED;
    if (rect.x < 0) rect.x = 0;
}

void Player::moveRight() {
    rect.x += PLAYER_SPEED;
    if (rect.x > SCREEN_WIDTH - rect.w) rect.x = SCREEN_WIDTH - rect.w;
}


int Player::getScore() const {
    return score;
}

void Player::addScore(int points) {
    score += points;
}

bool Player::isAlive() const {
    return alive;
}

void Player::kill() {
    alive = false;
}

void Player::setTextureID(const std::string& id) {
    textureID = id;
}
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL2/SDL.h>
#include "constants.h"
#include "texturemanager.h"

// Abstract GameObject base class
class GameObject {
protected:
    SDL_Rect rect;
    bool active;

public:
    GameObject(int x, int y, int w, int h);
    virtual ~GameObject() = default;

    // Pure virtual functions
    virtual void update() = 0;
    virtual void render(SDL_Renderer* renderer) const = 0;

    // Common methods
    const SDL_Rect& getRect() const;
    bool isActive() const;
    void setActive(bool isActive);
    bool checkCollision(const SDL_Rect& other) const;
};

// Cell is a grid-based game object
class Cell : public GameObject {
private:
    CellType type;
    bool collected;
    std::string textureID;

public:
    Cell(CellType t, int x, int y, int w, int h);

    void update() override;
    void render(SDL_Renderer* renderer) const override;
    
    CellType getType() const;
    bool isCollected() const;
    void collect();
    void setTextureID(const std::string& id);
    std::string& getTextureID();
};

// Player class
class Player : public GameObject {
private:
    int score;
    bool alive;
    std::string textureID;

    // Animation properties
    int currentFrame;
    int frameCount;    // Total frames in animation
    int animationSpeed; // Controls how fast the animation plays
    int frameCounter;   // Internal counter for timing

public:
    Player(int x, int y);
    
    void update() override;
    void render(SDL_Renderer* renderer) const override;
    
    void moveLeft();
    void moveRight();
    int getScore() const;
    void addScore(int points);
    bool isAlive() const;
    void kill();
    void setTextureID(const std::string& id);
};

#endif // GAMEOBJECT_H
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SDL2/SDL.h>
#include "constants.h"
#include "texturemanager.h"

// Abstract GameObject base class
class GameObject {
protected:
    SDL_Rect rect;              // Position and size
    bool active;                // Whether the object is active
    std::string textureID;      // Texture identifier for rendering

    // Animation properties
    int currentFrame;           // Current frame in animation
    int frameCount;             // Total frames in animation
    int animationSpeed;         // Controls how fast the animation plays (higher = slower)
    int frameCounter;           // Internal counter for timing

public:
    GameObject(int x, int y, int w, int h, const std::string ID, int frames, int speed);
    virtual ~GameObject() = default;

    
    void update(); // same function for all other class (Player and Cell)
    virtual void render(SDL_Renderer* renderer) const = 0;
    virtual void render(SDL_Renderer* renderer, const SDL_Rect& destRect) const = 0;

    // Common methods
    const SDL_Rect& getRect() const;
    bool isActive() const;
    void setActive(bool isActive);
    bool checkCollision(const SDL_Rect& other) const;
    void setTextureID(const std::string& id);
    const std::string& getTextureID() const;
};

// Cell is a grid-based game object
class Cell : public GameObject {
private:
    CellType type;
    bool collected;

public:
    Cell(CellType t, int x, int y, int w, int h);

    void render(SDL_Renderer* renderer) const override;
    void render(SDL_Renderer* renderer, const SDL_Rect& destRect) const override;
    
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

public:
    Player(int x, int y);
    
    void render(SDL_Renderer* renderer) const override;
    void render(SDL_Renderer* renderer, const SDL_Rect& destRect) const;
    
    void moveLeft();
    void moveRight();
    int getScore() const;
    void addScore(int points);
    bool isAlive() const;
    void kill();
   
};


#endif // GAMEOBJECT_H
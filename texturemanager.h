#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <map>
#include <iostream>

class TextureManager {
private:
    // Static instance for singleton pattern
    static TextureManager* instance;
    
    // Map to store loaded textures
    std::map<std::string, SDL_Texture*> textureMap;
    
    // Private constructor for singleton
    TextureManager() = default;
    
public:
    // Delete copy constructor and assignment operator
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    
    // Get singleton instance
    static TextureManager* Instance();
    
    // Initialize SDL_image
    bool init();
    
    // Load texture from file
    bool loadTexture(const std::string& fileName, const std::string& id, SDL_Renderer* renderer);
    
    // Draw texture (entire texture)
    void draw(const std::string& id, int x, int y, int width, int height, 
              SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void drawWhole(const std::string& id, int x, int y, int width, int height, 
        SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);
    
    // Draw frame from sprite sheet
    void drawFrame(const std::string& id, int x, int y, int width, int height, 
                  int currentRow, int currentFrame, SDL_Renderer* renderer, 
                  SDL_RendererFlip flip = SDL_FLIP_NONE);
    
    // Draw portion of a texture (source rectangle)
    void drawPortion(const std::string& id, const SDL_Rect& srcRect, const SDL_Rect& destRect,
                    SDL_Renderer* renderer, SDL_RendererFlip flip = SDL_FLIP_NONE);
    
    // Get texture by ID
    SDL_Texture* getTexture(const std::string& id);
    
    // Clear all loaded textures
    void clearTextures();
    
    // Clean up resources
    void clean();
};

// Shorthand for accessing the texture manager
typedef TextureManager TheTextureManager;

#endif // TEXTUREMANAGER_H
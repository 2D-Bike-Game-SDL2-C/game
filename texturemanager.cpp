#include "texturemanager.h"

// Initialize static instance to nullptr
TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::Instance() {
    // Create instance if it doesn't exist
    if (instance == nullptr) {
        instance = new TextureManager();
    }
    return instance;
}

bool TextureManager::init() {
    // Initialize SDL_image with PNG and JPG support
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    int inittedFlags = IMG_Init(imgFlags);
    
    if ((inittedFlags & imgFlags) != imgFlags) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " 
                  << IMG_GetError() << std::endl;
        return false;
    }
    
    return true;
}

bool TextureManager::loadTexture(const std::string& fileName, const std::string& id, SDL_Renderer* renderer) {
    // Load image from file
    SDL_Surface* tempSurface = IMG_Load(fileName.c_str());
    if (tempSurface == nullptr) {
        std::cerr << "Failed to load texture file: " << fileName 
                  << " Error: " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    
    if (texture == nullptr) {
        std::cerr << "Failed to create texture from surface. SDL Error: " 
                  << SDL_GetError() << std::endl;
        return false;
    }
    
    // Add texture to the map
    textureMap[id] = texture;
    return true;
}

void TextureManager::draw(const std::string& id, int x, int y, int width, int height, 
                         SDL_Renderer* renderer, SDL_RendererFlip flip) {
    // Source and destination rectangles
    SDL_Rect srcRect = {0, 0, width, height};
    SDL_Rect destRect = {x, y, width, height};
    
    // Render the texture
    SDL_RenderCopyEx(renderer, textureMap[id], &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::drawWhole(const std::string& id, int x, int y, int width, int height, 
    SDL_Renderer* renderer, SDL_RendererFlip flip)
{
    SDL_Rect destRect = {x, y, width, height};
    
    // Render the texture
    SDL_RenderCopyEx(renderer, textureMap[id], NULL, &destRect, 0, nullptr, flip);
}

void TextureManager::drawFrame(const std::string& id, int x, int y, int width, int height, 
                              int currentRow, int currentFrame, SDL_Renderer* renderer, 
                              SDL_RendererFlip flip) {
    // Calculate source rectangle based on frame and row
    SDL_Rect srcRect;
    srcRect.x = width * currentFrame;
    srcRect.y = height * currentRow;
    srcRect.w = width;
    srcRect.h = height;
    
    // Destination rectangle
    SDL_Rect destRect = {x, y, width, height};
    
    // Render the specific frame
    SDL_RenderCopyEx(renderer, textureMap[id], &srcRect, &destRect, 0, nullptr, flip);
}

void TextureManager::drawPortion(const std::string& id, const SDL_Rect& srcRect, const SDL_Rect& destRect,
                               SDL_Renderer* renderer, SDL_RendererFlip flip) {
    // Render the specific portion of the texture
    SDL_RenderCopyEx(renderer, textureMap[id], &srcRect, &destRect, 0, nullptr, flip);
}

SDL_Texture* TextureManager::getTexture(const std::string& id) {
    // Return the texture if it exists
    if (textureMap.find(id) != textureMap.end()) {
        return textureMap[id];
    }
    
    std::cerr << "Texture with ID: " << id << " not found!" << std::endl;
    return nullptr;
}

void TextureManager::clearTextures() {
    // Free all textures and clear the map
    for (auto& texture : textureMap) {
        if (texture.second != nullptr) {
            SDL_DestroyTexture(texture.second);
        }
    }
    
    textureMap.clear();
}

void TextureManager::clean() {
    // Clean up all loaded textures
    clearTextures();
    
    // Clean up the instance
    delete instance;
    instance = nullptr;
    
    // Quit SDL_image
    IMG_Quit();
}
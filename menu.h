#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <functional>
#include <string>
#include <memory>
#include "texturemanager.h"

// Forward declaration
class Game;

// MenuState enum to manage different states
enum class MenuState {
    MAIN_MENU,
    GAME_PLAYING,
    PAUSE_MENU,
    OPTIONS_MENU,
    GAME_OVER,
    LEVEL_COMPLETE,
    ABOUT  // Added for About screen
};

// Button class for menu items
class Button {
private:
    SDL_Rect rect;
    std::string text;
    bool selected;
    std::function<void()> clickHandler;
    
    // Colors
    SDL_Color normalColor;
    SDL_Color hoverColor;
    SDL_Color selectedColor;
    
    // Texture identifiers
    std::string buttonTextureID;
    bool hasTexture;

public:
    Button(int x, int y, int w, int h, const std::string& btnText, std::function<void()> handler);
    ~Button() = default;
    
    void update();
    void render(SDL_Renderer* renderer, TTF_Font* font);
    bool handleEvent(const SDL_Event& event, int mouseX, int mouseY);
    void setSelected(bool isSelected);
    bool isSelected() const;
    void setPosition(int x, int y);
    const SDL_Rect& getRect() const;
    void setTexture(const std::string& textureID);
};

// Menu base class
class Menu {
protected:
    std::vector<std::unique_ptr<Button>> buttons;
    int selectedIndex;
    Game* gameInstance;
    TTF_Font* menuFont;
    std::string backgroundTextureID;
    bool hasBackground;
    std::string title;
    
public:
    Menu(Game* game, const std::string& menuTitle);
    virtual ~Menu();
    
    virtual void init() = 0;
    virtual void update();
    virtual void render(SDL_Renderer* renderer);
    virtual bool handleEvent(const SDL_Event& event);
    
    void addButton(int x, int y, int w, int h, const std::string& text, std::function<void()> handler);
    void navigateUp();
    void navigateDown();
    void selectCurrent();
    void setBackground(const std::string& textureID);
};

// MainMenu implementation
class MainMenu : public Menu {
public:
    MainMenu(Game* game);
    ~MainMenu() = default;
    
    void init() override;
};

// PauseMenu implementation
class PauseMenu : public Menu {
public:
    PauseMenu(Game* game);
    ~PauseMenu() = default;
    
    void init() override;
};

// GameOverMenu implementation
class GameOverMenu : public Menu {
private:
    int finalScore;
    int finalDistance;
    
public:
    GameOverMenu(Game* game);
    ~GameOverMenu() = default;
    
    void init() override;
    void render(SDL_Renderer* renderer) override;
    void setResults(int score, int distance);
};

// OptionsMenu implementation
class OptionsMenu : public Menu {
private:
    bool musicEnabled;
    bool soundEnabled;
    int difficulty;
    
public:
    OptionsMenu(Game* game);
    ~OptionsMenu() = default;
    
    void init() override;
    void toggleMusic();
    void toggleSound();
    void increaseDifficulty();
    void decreaseDifficulty();
};

// AboutMenu implementation
class AboutMenu : public Menu {
public:
    AboutMenu(Game* game);
    ~AboutMenu() = default;
    
    void init() override;
    bool handleEvent(const SDL_Event& event) override;
    void render(SDL_Renderer* renderer) override;
};

#endif // MENU_H
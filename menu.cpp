#include "menu.h"
#include "game.h"
#include "constants.h"
#include <iostream>

// Button implementation
Button::Button(int x, int y, int w, int h, const std::string& btnText, std::function<void()> handler)
    : rect{x, y, w, h}, 
      text(btnText), 
      selected(false), 
      clickHandler(handler),
      normalColor{200, 200, 200, 255},
      hoverColor{255, 255, 255, 255},
      selectedColor{255, 215, 0, 255},
      hasTexture(false)
{}

void Button::update() {
    // Animation or other updates could go here
}

void Button::render(SDL_Renderer* renderer, TTF_Font* font) {
    // Draw button background
    if (hasTexture) {
        // Use texture for background
        TheTextureManager::Instance()->draw(
            buttonTextureID,
            rect.x, rect.y,
            rect.w, rect.h,
            renderer
        );
    } else {
        // Draw rectangle for background
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 200);
        SDL_RenderFillRect(renderer, &rect);
        
        // Draw border
        if (selected) {
            SDL_SetRenderDrawColor(renderer, selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
        } else {
            SDL_SetRenderDrawColor(renderer, normalColor.r, normalColor.g, normalColor.b, normalColor.a);
        }
        SDL_RenderDrawRect(renderer, &rect);
    }
    
    // Render text
    if (font) {
        SDL_Color textColor = selected ? selectedColor : normalColor;
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            
            // Center the text in the button
            SDL_Rect textRect = {
                rect.x + (rect.w - textSurface->w) / 2,
                rect.y + (rect.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h
            };
            
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            
            // Clean up
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
        }
    }
}

bool Button::handleEvent(const SDL_Event& event, int mouseX, int mouseY) {
    // Check if mouse is inside button
    bool inside = (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
                   mouseY >= rect.y && mouseY <= rect.y + rect.h);
    
    // Handle different mouse events
    if (event.type == SDL_MOUSEMOTION) {
        // Mouse hover effect
        if (inside) {
            selected = true;
            return true;
        } else {
            selected = false;
        }
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (inside && event.button.button == SDL_BUTTON_LEFT) {
            // Execute click handler
            if (clickHandler) {
                clickHandler();
            }
            return true;
        }
    }
    
    return false;
}

void Button::setSelected(bool isSelected) {
    selected = isSelected;
}

bool Button::isSelected() const {
    return selected;
}

void Button::setPosition(int x, int y) {
    rect.x = x;
    rect.y = y;
}

const SDL_Rect& Button::getRect() const {
    return rect;
}

void Button::setTexture(const std::string& textureID) {
    buttonTextureID = textureID;
    hasTexture = true;
}

// Menu implementation
Menu::Menu(Game* game, const std::string& menuTitle) 
    : selectedIndex(0), 
      gameInstance(game),
      menuFont(nullptr),
      hasBackground(false),
      title(menuTitle) {
    // Load font
    menuFont = TTF_OpenFont("assets/arial.ttf", 28);
    if (!menuFont) {
        std::cerr << "Font load error: " << TTF_GetError() << std::endl;
    }
}

Menu::~Menu() {
    if (menuFont) {
        TTF_CloseFont(menuFont);
        menuFont = nullptr;
    }
}

void Menu::update() {
    // Update all buttons
    for (auto& button : buttons) {
        button->update();
    }
}

void Menu::render(SDL_Renderer* renderer) {
    // Draw background if available
    if (hasBackground) {
        TheTextureManager::Instance()->draw(
            backgroundTextureID,
            0, 0,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            renderer
        );
    } else {
        // Draw semi-transparent background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &bgRect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
    
    // Render title
    if (menuFont && !title.empty()) {
        SDL_Color titleColor = {255, 255, 255, 255};
        SDL_Surface* titleSurface = TTF_RenderText_Solid(menuFont, title.c_str(), titleColor);
        if (titleSurface) {
            SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
            
            SDL_Rect titleRect = {
                (SCREEN_WIDTH - titleSurface->w) / 2,
                50,
                titleSurface->w,
                titleSurface->h
            };
            
            SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
            
            // Clean up
            SDL_FreeSurface(titleSurface);
            SDL_DestroyTexture(titleTexture);
        }
    }
    
    // Render all buttons
    for (auto& button : buttons) {
        button->render(renderer, menuFont);
    }
}

bool Menu::handleEvent(const SDL_Event& event) {
    // Get mouse position
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    
    // Check all buttons
    for (int i = 0; i < buttons.size(); ++i) {
        if (buttons[i]->handleEvent(event, mouseX, mouseY)) {
            selectedIndex = i;
            return true;
        }
    }
    
    // Handle keyboard navigation
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                navigateUp();
                return true;
            case SDLK_DOWN:
                navigateDown();
                return true;
            case SDLK_RETURN:
            case SDLK_SPACE:
                selectCurrent();
                return true;
        }
    }
    
    return false;
}

void Menu::addButton(int x, int y, int w, int h, const std::string& text, std::function<void()> handler) {
    buttons.push_back(std::make_unique<Button>(x, y, w, h, text, handler));
    
    // Select the first button by default
    if (buttons.size() == 1) {
        buttons[0]->setSelected(true);
    }
}

void Menu::navigateUp() {
    // Deselect current button
    buttons[selectedIndex]->setSelected(false);
    
    // Move selection up
    selectedIndex = (selectedIndex + buttons.size() - 1) % buttons.size();
    
    // Select new button
    buttons[selectedIndex]->setSelected(true);
}

void Menu::navigateDown() {
    // Deselect current button
    buttons[selectedIndex]->setSelected(false);
    
    // Move selection down
    selectedIndex = (selectedIndex + 1) % buttons.size();
    
    // Select new button
    buttons[selectedIndex]->setSelected(true);
}

void Menu::selectCurrent() {
    // Trigger click handler for current button
    if (selectedIndex >= 0 && selectedIndex < buttons.size()) {
        buttons[selectedIndex]->handleEvent(SDL_Event{SDL_MOUSEBUTTONDOWN}, 
                                          buttons[selectedIndex]->getRect().x + 1, 
                                          buttons[selectedIndex]->getRect().y + 1);
    }
}

void Menu::setBackground(const std::string& textureID) {
    backgroundTextureID = textureID;
    hasBackground = true;
}

// MainMenu implementation
MainMenu::MainMenu(Game* game) : Menu(game, "ENDLESS RUNNER") {
    init();
}

void MainMenu::init() {
    // Create buttons
    int buttonWidth = 300;
    int buttonHeight = 60;
    int startY = SCREEN_HEIGHT / 2 - 30;
    int spacing = 80;
    
    // Start Game button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY,
        buttonWidth,
        buttonHeight,
        "Start Game",
        [this]() { 
            if (gameInstance) gameInstance->setGameState(MenuState::GAME_PLAYING); 
        }
    );
    
    // Options button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY + spacing,
        buttonWidth,
        buttonHeight,
        "Options",
        [this]() { 
            if (gameInstance) gameInstance->setGameState(MenuState::OPTIONS_MENU); 
        }
    );
    
    // Exit button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY + spacing * 2,
        buttonWidth,
        buttonHeight,
        "Exit Game",
        [this]() { 
            if (gameInstance) gameInstance->quit(); 
        }
    );
}

// PauseMenu implementation
PauseMenu::PauseMenu(Game* game) : Menu(game, "GAME PAUSED") {
    init();
}

void PauseMenu::init() {
    // Create buttons
    int buttonWidth = 300;
    int buttonHeight = 60;
    int startY = SCREEN_HEIGHT / 2 - 30;
    int spacing = 80;
    
    // Resume Game button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY,
        buttonWidth,
        buttonHeight,
        "Resume Game",
        [this]() { 
            if (gameInstance) gameInstance->setGameState(MenuState::GAME_PLAYING); 
        }
    );
    
    // Main Menu button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY + spacing,
        buttonWidth,
        buttonHeight,
        "Main Menu",
        [this]() { 
            if (gameInstance) gameInstance->setGameState(MenuState::MAIN_MENU); 
        }
    );
    
    // Exit button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY + spacing * 2,
        buttonWidth,
        buttonHeight,
        "Exit Game",
        [this]() { 
            if (gameInstance) gameInstance->quit(); 
        }
    );
}

// GameOverMenu implementation
GameOverMenu::GameOverMenu(Game* game) 
    : Menu(game, "GAME OVER"), finalScore(0), finalDistance(0) {
    init();
}

void GameOverMenu::init() {
    // Create buttons
    int buttonWidth = 300;
    int buttonHeight = 60;
    int startY = SCREEN_HEIGHT / 2 + 50;
    int spacing = 80;
    
    // Retry button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY,
        buttonWidth,
        buttonHeight,
        "Retry",
        [this]() { 
            if (gameInstance) gameInstance->restart(); 
        }
    );
    
    // Main Menu button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY + spacing,
        buttonWidth,
        buttonHeight,
        "Main Menu",
        [this]() { 
            if (gameInstance) gameInstance->setGameState(MenuState::MAIN_MENU); 
        }
    );
}

void GameOverMenu::render(SDL_Renderer* renderer) {
    // Call parent render method
    Menu::render(renderer);
    
    // Render score and distance
    if (menuFont) {
        SDL_Color textColor = {255, 255, 255, 255};
        
        // Score text
        std::string scoreText = "Score: " + std::to_string(finalScore);
        SDL_Surface* scoreSurface = TTF_RenderText_Solid(menuFont, scoreText.c_str(), textColor);
        if (scoreSurface) {
            SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
            
            SDL_Rect scoreRect = {
                (SCREEN_WIDTH - scoreSurface->w) / 2,
                SCREEN_HEIGHT / 2 - 60,
                scoreSurface->w,
                scoreSurface->h
            };
            
            SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreRect);
            
            // Clean up
            SDL_FreeSurface(scoreSurface);
            SDL_DestroyTexture(scoreTexture);
        }
        
        // Distance text
        std::string distanceText = "Distance: " + std::to_string(finalDistance);
        SDL_Surface* distSurface = TTF_RenderText_Solid(menuFont, distanceText.c_str(), textColor);
        if (distSurface) {
            SDL_Texture* distTexture = SDL_CreateTextureFromSurface(renderer, distSurface);
            
            SDL_Rect distRect = {
                (SCREEN_WIDTH - distSurface->w) / 2,
                SCREEN_HEIGHT / 2 - 20,
                distSurface->w,
                distSurface->h
            };
            
            SDL_RenderCopy(renderer, distTexture, nullptr, &distRect);
            
            // Clean up
            SDL_FreeSurface(distSurface);
            SDL_DestroyTexture(distTexture);
        }
    }
}

void GameOverMenu::setResults(int score, int distance) {
    finalScore = score;
    finalDistance = distance;
}

// OptionsMenu implementation
OptionsMenu::OptionsMenu(Game* game) 
    : Menu(game, "OPTIONS"), musicEnabled(true), soundEnabled(true), difficulty(1) {
    init();
}

void OptionsMenu::init() {
    // Create buttons
    int buttonWidth = 300;
    int buttonHeight = 60;
    int startY = SCREEN_HEIGHT / 2 - 90;
    int spacing = 80;
    
    // Music toggle button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY,
        buttonWidth,
        buttonHeight,
        "Music: ON",
        [this]() { 
            toggleMusic(); 
        }
    );
    
    // Sound effects toggle button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY + spacing,
        buttonWidth,
        buttonHeight,
        "Sound Effects: ON",
        [this]() { 
            toggleSound(); 
        }
    );
    
    // Difficulty button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY + spacing * 2,
        buttonWidth,
        buttonHeight,
        "Difficulty: Normal",
        [this]() { 
            increaseDifficulty(); 
        }
    );
    
    // Back button
    addButton(
        (SCREEN_WIDTH - buttonWidth) / 2,
        startY + spacing * 3,
        buttonWidth,
        buttonHeight,
        "Back",
        [this]() { 
            if (gameInstance) gameInstance->setGameState(MenuState::MAIN_MENU); 
        }
    );
}

void OptionsMenu::toggleMusic() {
    musicEnabled = !musicEnabled;
    buttons[0]->setPosition(buttons[0]->getRect().x, buttons[0]->getRect().y);
    
    // TODO: Actually toggle game music
    std::string buttonText = "Music: ";
    buttonText += (musicEnabled ? "ON" : "OFF");
    buttons[0] = std::make_unique<Button>(
        buttons[0]->getRect().x,
        buttons[0]->getRect().y,
        buttons[0]->getRect().w,
        buttons[0]->getRect().h,
        buttonText,
        [this]() { toggleMusic(); }
    );
    buttons[0]->setSelected(true);
}

void OptionsMenu::toggleSound() {
    soundEnabled = !soundEnabled;
    
    // TODO: Actually toggle game sound effects
    std::string buttonText = "Sound Effects: ";
    buttonText += (soundEnabled ? "ON" : "OFF");
    buttons[1] = std::make_unique<Button>(
        buttons[1]->getRect().x,
        buttons[1]->getRect().y,
        buttons[1]->getRect().w,
        buttons[1]->getRect().h,
        buttonText,
        [this]() { toggleSound(); }
    );
    buttons[1]->setSelected(false);
}

void OptionsMenu::increaseDifficulty() {
    difficulty = (difficulty % 3) + 1;
    
    std::string difficultyText;
    switch (difficulty) {
        case 1: difficultyText = "Easy"; break;
        case 2: difficultyText = "Normal"; break;
        case 3: difficultyText = "Hard"; break;
    }
    
    std::string buttonText = "Difficulty: " + difficultyText;
    buttons[2] = std::make_unique<Button>(
        buttons[2]->getRect().x,
        buttons[2]->getRect().y,
        buttons[2]->getRect().w,
        buttons[2]->getRect().h,
        buttonText,
        [this]() { increaseDifficulty(); }
    );
    buttons[2]->setSelected(false);
    
    // TODO: Set game difficulty
}

void OptionsMenu::decreaseDifficulty() {
    difficulty = (difficulty + 1) % 3 + 1;
    
    std::string difficultyText;
    switch (difficulty) {
        case 1: difficultyText = "Easy"; break;
        case 2: difficultyText = "Normal"; break;
        case 3: difficultyText = "Hard"; break;
    }
    
    std::string buttonText = "Difficulty: " + difficultyText;
    buttons[2] = std::make_unique<Button>(
        buttons[2]->getRect().x,
        buttons[2]->getRect().y,
        buttons[2]->getRect().w,
        buttons[2]->getRect().h,
        buttonText,
        [this]() { decreaseDifficulty(); }
    );
    buttons[2]->setSelected(false);
    
    // TODO: Set game difficulty
}
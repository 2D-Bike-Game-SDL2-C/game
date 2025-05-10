# Endless Runner Game

## Gameplay Video

You can watch the gameplay video below:

![Gameplay Video](https://youtu.be/uDZLCjpUWxQ)

## Overview

This is an Endless Runner game developed using **C++** and the **SDL2** library. The game features a player navigating through a scrolling grid-based map, avoiding obstacles, collecting coins, and aiming to reach a finish line. It includes multiple menus (Main Menu, Pause Menu, Game Over Menu, Options Menu, and About Menu) to provide a complete user experience.

## Features

- **Gameplay**: Control a player to dodge obstacles, collect coins, and reach the finish line.
- **Dynamic Map**: Procedurally generated map with increasing difficulty.
- **Menus**:
  - **Main Menu**: Start the game, access options, view the about page, or exit.
  - **Pause Menu**: Resume, return to main menu, or quit.
  - **Game Over Menu**: Retry or return to main menu.
  - **Options Menu**: Toggle music, sound effects, and adjust difficulty.
  - **About Menu**: View game information.
- **UI**: Displays score and distance traveled during gameplay.
- **Textures and Animations**: Sprite sheets for player, obstacles, and coins with animation support.
- **Cross-Platform**: Built with SDL2 for compatibility across Windows, macOS, and Linux.

## Prerequisites

To build and run the game, you need the following:

- **C++ Compiler**: GCC, Clang, or MSVC with C++11 or later support.
- **SDL2 Libraries**:
  - SDL2
  - SDL2_image
  - SDL2_ttf
- **Asset Files**: Ensure the `assets/` folder contains:
  - Textures: `player`, `obstacle`, `coin`, `finish`, `background`, `menu_background`, `about_background`
  - Font: `arial.ttf`

## Controls

- **Arrow Keys or A/D**: Move the player left or right.
- **Escape**: Pause the game.
- **Mouse**: Interact with menu buttons.
- **R**: Restart the game after completing a level.

## Project Structure

### Source Files

- **`main.cpp`**: Entry point of the game.
- **`game.h/cpp`**: Core game logic and state management.
- **`gamemap.h/cpp`**: Manages the scrolling grid-based map.
- **`gameobject.h/cpp`**: Defines game objects (Player, Cell).
- **`texturemanager.h/cpp`**: Handles texture loading and rendering.
- **`menu.h/cpp`**: Implements menu system and button interactions.
- **`constants.h`**: Game constants (screen size, grid size, etc.).

### Assets

- **`assets/`**: Contains textures and font files.
  - `player.png`
  - `obstacle.png`
  - `coin.png`
  - `finish.png`
  - `background.png`
  - `menu_background.png`
  - `about_background.png`
  - `arial.ttf`

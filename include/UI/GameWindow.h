// UI/GameWindow.h
#pragma once
#include "../Core/GameEngine.h"
#include "Input.h"
#include <string>

// Note: If you use SFML, you would include <SFML/Graphics.hpp> here 
// and use sf::RenderWindow. I am keeping it framework-independent for now.

class GameWindow {
private:
    int width;
    int height;
    Input inputTranslator;
    
    // Framework-specific window pointer goes here (e.g., sf::RenderWindow* window)

    // Helper functions to keep rendering clean
    void drawGrid();
    void drawWalls(const std::vector<Wall>& walls);
    void drawPawns(const Player& p1, const Player& p2);
    void drawHUD(const GameEngine& engine); // Shows whose turn it is & wall counts
    void highlightValidMoves(const std::vector<Position>& validMoves); 

public:
    GameWindow(int winWidth, int winHeight, const std::string& title);
    ~GameWindow(); 

    bool isOpen() const;
    
    // Captures mouse clicks and tells the GameEngine what the user wants to do
    void handleEvents(GameEngine& engine); 
    
    // Clears the screen, calls all draw helpers, and displays the frame
    void render(const GameEngine& engine); 
    
    // Triggers the end-game screen
    void displayWinner(PlayerId winner);
};
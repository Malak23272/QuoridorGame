// UI/Input.h
#pragma once
#include "../Core/Types.h"

// Translates raw screen pixels (e.g., 800x600) into game grid coordinates (0-8)
class Input {
private:
    int windowWidth;
    int windowHeight;
    int boardPixelSize; // Total size of the graphical board
    int cellSize;       // Pixel size of a single 9x9 square
    int wallGapSize;    // Pixel size of the gap between squares where walls go

public:
    Input(int width, int height, int boardSize);

    // Determines if a mouse click was inside a square (pawn move)
    bool isPawnMoveClick(int mouseX, int mouseY) const;
    
    // Determines if a mouse click was in the gap between squares (wall placement)
    bool isWallPlacementClick(int mouseX, int mouseY) const;

    // Translates the pixel click into a grid Position
    Position getGridPositionFromMouse(int mouseX, int mouseY) const;
    
    // Translates the pixel click into a Wall orientation and start position
    Wall getWallFromMouse(int mouseX, int mouseY) const;
};
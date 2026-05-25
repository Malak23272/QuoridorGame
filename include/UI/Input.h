// UI/Input.h
#pragma once
#include "../Core/Types.h"

// Translates raw screen pixels (e.g., 800x600) into game grid coordinates (0-8)
class Input {
private:
    int windowWidth = 0;
    int windowHeight = 0;
    int boardPixelSize = 0; // Total pixel size of the drawing board
    int cellSize = 0;       // Pixel size of a single cell
    int wallGapSize = 0;    // Pixel size of the gap between cells where walls go
    int margin = 0;         // Offset from widget edge to board start

    void recalculate();

public:
    Input() = default;
    Input(int width, int height, int /*boardSize*/);

    // Reinitialize for new widget dimensions (call on resize)
    void resize(int newWidth, int newHeight);

    // Click classification
    bool isPawnMoveClick(int mouseX, int mouseY) const;
    bool isWallPlacementClick(int mouseX, int mouseY) const;

    // Coordinate translation
    Position getGridPositionFromMouse(int mouseX, int mouseY) const;
    Wall getWallFromMouse(int mouseX, int mouseY) const;

    // Getters for rendering
    int getCellSize() const { return cellSize; }
    int getGapSize() const { return wallGapSize; }
    int getMargin() const { return margin; }
    int getBoardPixelSize() const { return boardPixelSize; }
};
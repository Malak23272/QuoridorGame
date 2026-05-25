#include "UI/Input.h"
#include <algorithm>
#include <cmath>

Input::Input(int width, int height, int /*boardSize*/) {
    windowWidth = width;
    windowHeight = height;
    recalculate();
}

void Input::recalculate() {
    int dim = std::min(windowWidth, windowHeight);
    margin = std::max(1, dim / 20);
    boardPixelSize = dim - 2 * margin;

    cellSize = boardPixelSize / 11;
    wallGapSize = std::max(1, cellSize / 4);
}

void Input::resize(int newWidth, int newHeight) {
    windowWidth = newWidth;
    windowHeight = newHeight;
    recalculate();
}

// Returns true if the click falls within any cell square on the board.
bool Input::isPawnMoveClick(int mouseX, int mouseY) const {
    int step = cellSize + wallGapSize;
    if (mouseX < margin || mouseY < margin) return false;

    int dx = mouseX - margin;
    int dy = mouseY - margin;

    int gx = dx / step;
    int gy = dy / step;

    if (gx < 0 || gx >= 9 || gy < 0 || gy >= 9) return false;

    int rx = dx - gx * step;
    int ry = dy - gy * step;

    return (rx < cellSize && ry < cellSize);
}

// Returns true if the click falls in a gap region (wall placement zone).
bool Input::isWallPlacementClick(int mouseX, int mouseY) const {
    if (isPawnMoveClick(mouseX, mouseY)) return false;

    int step = cellSize + wallGapSize;
    if (mouseX < margin || mouseY < margin) return false;

    int dx = mouseX - margin;
    int dy = mouseY - margin;

    int gx = dx / step;
    int gy = dy / step;

    if (gx < 0 || gx >= 9 || gy < 0 || gy >= 9) return false;

    return true;
}

// Converts a cell click to a grid Position.
Position Input::getGridPositionFromMouse(int mouseX, int mouseY) const {
    int step = cellSize + wallGapSize;
    int gx = (mouseX - margin) / step;
    int gy = (mouseY - margin) / step;
    return {std::clamp(gx, 0, 8), std::clamp(gy, 0, 8)};
}

// Converts a gap click to a Wall structure.
// Determines horizontal vs vertical based on which gap region was clicked.
Wall Input::getWallFromMouse(int mouseX, int mouseY) const {
    int step = cellSize + wallGapSize;
    int dx = mouseX - margin;
    int dy = mouseY - margin;

    int gx = dx / step;
    int gy = dy / step;
    int rx = dx - gx * step;
    int ry = dy - gy * step;

    Wall wall;
    // In the intersection region (both rx and ry in gap), pick the nearer orientation.
    if (ry >= cellSize && (rx < cellSize || ry >= rx)) {
        // Horizontal gap below row gy
        wall.orientation = WallOrientation::HORIZONTAL;
        wall.topLeft.x = std::clamp(gx, 0, 7);
        wall.topLeft.y = std::clamp(gy, 0, 7);
    } else {
        // Vertical gap right of column gx
        wall.orientation = WallOrientation::VERTICAL;
        wall.topLeft.x = std::clamp(gx, 0, 7);
        wall.topLeft.y = std::clamp(gy, 0, 7);
    }
    return wall;
}

#pragma once

// Represents a coordinate on the 9x9 grid (0 to 8)
struct Position {
    int x;
    int y;

    // Operator overload to easily compare if two positions are the same
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// Walls can only be placed in two directions
enum class WallOrientation {
    HORIZONTAL,
    VERTICAL
};

// A wall physically spans two squares. 
// 'topLeft' represents the coordinate of the top-leftmost square it touches.
struct Wall {
    Position topLeft;
    WallOrientation orientation;
};

// Identifies the players
enum class PlayerId {
    PLAYER_1, // Starts at bottom row (y=8), goal is top row (y=0)
    PLAYER_2  // Starts at top row (y=0), goal is bottom row (y=8)
};
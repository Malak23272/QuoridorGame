#pragma once
#include <vector>
#include "Types.h"
#include "Player.h"

class Board {
private:
    static const int BOARD_SIZE = 9;
    std::vector<Wall> placedWalls;

    // Helper to check if a specific edge is blocked by a wall
    bool isEdgeBlocked(Position from, Position to) const;

public:
    Board();

    // Validates if a pawn move is legal (orthogonal, no walls blocking, valid jumps)
    bool isValidPawnMove(Position current, Position target, Position opponentPos) const;
    
    // Validates if a wall overlaps another wall or goes out of bounds
    bool isValidWallPlacement(Wall wall) const;
    
    // This is where competitive programming experience will shine.
    // It requires a classic graph traversal (BFS or DFS) to guarantee 
    // that placing this new wall doesn't trap either player.
    bool doesWallBlockPath(Wall newWall, const Player& p1, const Player& p2) const;

    // Applies the wall to the board
    void placeWall(Wall wall);
    
    // Getters for the UI (to draw them) and AI (to evaluate the board)
    const std::vector<Wall>& getWalls() const;
    int getSize() const { return BOARD_SIZE; }
};
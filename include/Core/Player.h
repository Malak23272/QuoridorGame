#pragma once
#include "Types.h"

class Player {
private:
    PlayerId id;
    Position currentPosition;
    int wallsRemaining;
    int goalRow; // The y-coordinate this player needs to reach to win

public:
    Player(PlayerId id, Position startPos, int goalRow);
    
    // Getters
    PlayerId getId() const;
    Position getPosition() const;
    int getWallsRemaining() const;
    int getGoalRow() const;

    // State modifiers
    void setPosition(Position newPos);
    bool useWall(); // Decrements wall count, returns false if out of walls
    bool hasWon() const;
};
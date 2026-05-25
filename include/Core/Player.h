#pragma once
#include "Types.h"

class Player {
private:
    PlayerId id;
    Position currentPosition;
    int wallsRemaining;
    int goalRow; // The y-coordinate this player needs to reach to win

public:
    Player();
    Player(PlayerId id);
    
    // Getters
    PlayerId getId() const;
    Position getPosition() const;
    int getWallsRemaining() const;
    int getGoalRow() const;

    // State modifiers
    void setPosition(Position newPos);
    void setID(PlayerId id);
    bool useWall(); // Decrements wall count, returns false if out of walls
    bool hasWon() const;
    void loadPlayerState(Position pos, int walls);
};
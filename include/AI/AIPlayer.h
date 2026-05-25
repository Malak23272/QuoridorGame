// AI/AIPlayer.h
#pragma once
#include "Minmax.h"
#include "../Core/GameEngine.h"

// Tying into your bonus marks for difficulty levels!
enum class AIDifficulty {
    EASY = 1,   // Looks 1 move ahead (mostly greedy)
    MEDIUM = 2, // Looks 2 moves ahead
    HARD = 4    // Looks 3-4 moves ahead (Requires highly optimized code!)
};

class AIPlayer {
  private:
    Minimax brain;
    PlayerId id;

public:
    AIPlayer(PlayerId playerId, AIDifficulty difficulty);
    
    // Called by main.cpp during the AI's turn. 
    // It reads the engine state, asks Minimax for a move, and executes it.
    void takeTurn(GameEngine& engine);
};
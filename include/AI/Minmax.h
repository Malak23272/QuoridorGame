// AI/Minimax.h
#pragma once
#include "../Core/Board.h"
#include "../Core/Player.h"
#include <vector>

// Encapsulates a decision made by the AI
struct Move {
    bool isWallPlacement;
    Position pawnMove;   // Used if isWallPlacement is false
    Wall wallToPlace;    // Used if isWallPlacement is true
    int evaluationScore; // Internal score used by the Minimax algorithm
};

class Minimax {
private:
    int maxSearchDepth;
    PlayerId aiPlayerId;

    // The heuristic function: This is the "secret sauce" of your AI.
    // It should calculate: (Opponent's shortest path to goal) - (AI's shortest path to goal)
    int evaluateBoardState(const Board& board, const Player& aiPlayer, const Player& opponent) const;

    // Generates all legal pawn moves AND valid wall placements for a given state
    std::vector<Move> generateAllLegalMoves(const Board& board, const Player& activePlayer) const;

    // The recursive alpha-beta pruning function
    int minimaxRecursive(Board currentBoard, Player currentPlayer, Player opponentPlayer, 
                         int depth, int alpha, int beta, bool isMaximizing);

public:
    Minimax(int depth, PlayerId id);

    // Kicks off the recursive search and returns the best calculated move
    Move calculateBestMove(const Board& board, const Player& aiPlayer, const Player& opponent);
};
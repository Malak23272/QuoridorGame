#pragma once
#include "Board.h"
#include "Player.h"
#include "Types.h"

class GameEngine {
private:
    Board board;
    Player player1;
    Player player2;
    PlayerId currentPlayerTurn;

    // Internal helper to handle swapping turns
    void switchTurn();

public:
    // Initializes the board, gives 10 walls to each player, sets starting positions
    GameEngine(); 

    // Actions (Called by UI clicks or AI decisions)
    // These functions return false if the move was illegal, true if successful
    bool movePawn(Position newPos);
    bool placeWall(Wall wall);

    // Getters (Used by the UI to draw the screen, and AI to calculate moves)
    const Board& getBoard() const;
    const Player& getPlayer1() const;
    const Player& getPlayer2() const;
    PlayerId getCurrentTurn() const;
    bool isGameOver() const;
    PlayerId getWinner() const;
};

void GameEngine::switchTurn(){
    if(currentPlayerTurn == PlayerId::PLAYER_1) currentPlayerTurn = PlayerId::PLAYER_2;
    else currentPlayerTurn = PlayerId::PLAYER_1;
}
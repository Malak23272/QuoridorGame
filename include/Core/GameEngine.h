#pragma once
#include "Board.h"
#include "Player.h"
#include "Types.h"
#include <fstream>
#include <string>

class GameEngine {
private:
    Board board;
    Player player1;
    Player player2;
    PlayerId currentPlayerTurn;
    mutable std::string lastError;

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

    // Returns the reason the last movePawn/placeWall call failed
    std::string getLastError() const { return lastError; }

    //Saving/Loading Feature
    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);
};

inline void GameEngine::switchTurn(){
    if(currentPlayerTurn == PlayerId::PLAYER_1) currentPlayerTurn = PlayerId::PLAYER_2;
    else currentPlayerTurn = PlayerId::PLAYER_1;
}
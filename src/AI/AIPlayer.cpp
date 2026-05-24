#include "AIPlayer.h"

AIPlayer::AIPlayer(PlayerId playerId, AIDifficulty difficulty) : 
brain(int(difficulty), playerId), id(playerId){}


void AIPlayer::takeTurn(GameEngine& engine){
    const Player& aiPlayer = (id == PlayerId::PLAYER_1)? engine.getPlayer1(): engine.getPlayer2();
    const Player& human = (id == PlayerId::PLAYER_2)? engine.getPlayer2() : engine.getPlayer1();
    

    Board board = engine.getBoard();
    
    Move BestMove = brain.calculateBestMove(board, aiPlayer, human);
    //check move type:
    if(BestMove.isWallPlacement){
        engine.placeWall(BestMove.wallToPlace);
    }
    else{
        engine.movePawn(BestMove.pawnMove);
    }

}
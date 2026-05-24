#include "GameEngine.h"

GameEngine::GameEngine(): board(), player1(PlayerId::PLAYER_1), player2(PlayerId::PLAYER_2){
    currentPlayerTurn = PlayerId::PLAYER_1;
}

bool GameEngine::movePawn(Position newPos){
    if(currentPlayerTurn == PlayerId::PLAYER_1){
        return board.isValidPawnMove(player1.getPosition(), newPos, player2.getPosition());
    }
    else {
        return board.isValidPawnMove(player2.getPosition(), newPos, player1.getPosition());
    } 
}

bool GameEngine::placeWall(Wall wall){
    if(currentPlayerTurn == PlayerId::PLAYER_1){
        if(player1.getWallsRemaining()){
            if(board.isValidWallPlacement(wall) && !board.doesWallBlockPath(wall, player1, player2)){
            board.placeWall(wall);
            player1.useWall();
            return true;
            }
        }
    }
    else{
        if(player2.getWallsRemaining()){
            if(board.isValidWallPlacement(wall) && !board.doesWallBlockPath(wall, player1, player2)){
            board.placeWall(wall);
            player2.useWall();
            return true;
            }
        }
    }
    
    return false;
}


bool GameEngine::isGameOver() const{
    if(player1.hasWon() || player2.hasWon()){
        return true;
    }
    return false;
}


PlayerId GameEngine::getWinner() const{
    if(isGameOver()){
        if(player1.hasWon()) return PlayerId::PLAYER_1;
        else return PlayerId::PLAYER_2;
    }
    return PlayerId();
}

const Board& GameEngine::getBoard() const{
    return board;
}

const Player& GameEngine::getPlayer1() const{
    return player1;
}
const Player& GameEngine::getPlayer2() const{
    return player2;
}

PlayerId GameEngine::getCurrentTurn() const{
    return currentPlayerTurn;
}
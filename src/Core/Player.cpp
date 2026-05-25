#include "Player.h"

Player::Player() : id(PlayerId::PLAYER_1), wallsRemaining(10) {
    currentPosition.x = 4;
    currentPosition.y = 8;
    goalRow = 0;
}

Player::Player(PlayerId id): id(id), wallsRemaining(10){
    if(id == PlayerId::PLAYER_1){
        currentPosition.x = 4;
        currentPosition.y = 8;
        goalRow = 0;
    }
    else{
        currentPosition.x = 4;
        currentPosition.y = 0;
        goalRow = 8;
    }
}

void Player::setID(PlayerId id){
    this->id = id;
}

void Player::setPosition(Position newPos){
    currentPosition = newPos;
}

bool Player::useWall(){
    wallsRemaining--;
    return wallsRemaining;
}

bool Player::hasWon() const{
    return currentPosition.y == goalRow;
}


PlayerId Player::getId() const{
    return id;
}

Position Player::getPosition() const{
    return currentPosition;
}

int Player::getWallsRemaining() const{
    return wallsRemaining;
}

int Player::getGoalRow() const{
    return goalRow;
}

void Player::loadPlayerState(Position pos, int walls) {
    currentPosition = pos;
    wallsRemaining = walls;
}
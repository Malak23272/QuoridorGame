#include "GameEngine.h"

GameEngine::GameEngine(): board(), player1(PlayerId::PLAYER_1), player2(PlayerId::PLAYER_2){
    currentPlayerTurn = PlayerId::PLAYER_1;
}

bool GameEngine::movePawn(Position newPos){
    if(currentPlayerTurn == PlayerId::PLAYER_1){
        if(board.isValidPawnMove(player1.getPosition(), newPos, player2.getPosition())){
            player1.setPosition(newPos);
            switchTurn();
            lastError.clear();
            return true;
        }
        lastError = "Invalid pawn move (blocked by wall or out of bounds).";
    }
    else {
        if(board.isValidPawnMove(player2.getPosition(), newPos, player1.getPosition())){
            player2.setPosition(newPos);
            switchTurn();
            lastError.clear();
            return true;
        }
        lastError = "Invalid pawn move (blocked by wall or out of bounds).";
    } 
    return false;
}

bool GameEngine::placeWall(Wall wall){
    if(currentPlayerTurn == PlayerId::PLAYER_1){
        if(player1.getWallsRemaining()){
            if(board.isValidWallPlacement(wall) && !board.doesWallBlockPath(wall, player1, player2)){
                board.placeWall(wall);
                player1.useWall();
                switchTurn();
                lastError.clear();
                return true;
            }
            if (!board.isValidWallPlacement(wall))
                lastError = "Invalid wall placement (out of bounds or overlaps another wall).";
            else
                lastError = "Wall would block all paths to the goal.";
        } else {
            lastError = "No walls remaining.";
        }
    }
    else{
        if(player2.getWallsRemaining()){
            if(board.isValidWallPlacement(wall) && !board.doesWallBlockPath(wall, player1, player2)){
                board.placeWall(wall);
                player2.useWall();
                switchTurn();
                lastError.clear();
                return true;
            }
            if (!board.isValidWallPlacement(wall))
                lastError = "Invalid wall placement (out of bounds or overlaps another wall).";
            else
                lastError = "Wall would block all paths to the goal.";
        } else {
            lastError = "No walls remaining.";
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



bool GameEngine::saveGame(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) return false; // File error protection

    // 1. Save Current Turn
    outFile << static_cast<int>(currentPlayerTurn) << "\n";

    // 2. Save Player 1 Data (X, Y, Walls)
    outFile << player1.getPosition().x << " " << player1.getPosition().y << " " 
            << player1.getWallsRemaining() << "\n";

    // 3. Save Player 2 Data
    outFile << player2.getPosition().x << " " << player2.getPosition().y << " " 
            << player2.getWallsRemaining() << "\n";

    // 4. Save Board Walls
    const auto& walls = board.getWalls();
    outFile << walls.size() << "\n"; // Print wall count first
    for (const auto& wall : walls) {
        outFile << wall.topLeft.x << " " << wall.topLeft.y << " " 
                << static_cast<int>(wall.orientation) << "\n";
    }

    outFile.close();
    return true;
}


bool GameEngine::loadGame(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) return false; // File not found protection

    // 1. Read and restore Current Turn
    int turnInt;
    inFile >> turnInt;
    currentPlayerTurn = static_cast<PlayerId>(turnInt);

    // 2. Read and restore Player 1 State
    int p1X, p1Y, p1Walls;
    inFile >> p1X >> p1Y >> p1Walls;
    player1.loadPlayerState({p1X, p1Y}, p1Walls);

    // 3. Read and restore Player 2 State
    int p2X, p2Y, p2Walls;
    inFile >> p2X >> p2Y >> p2Walls;
    player2.loadPlayerState({p2X, p2Y}, p2Walls);

    // 4. Read and restore Board Walls
    board.clearBoard(); // Wipe existing runtime walls
    int wallCount;
    inFile >> wallCount;

    for (int i = 0; i < wallCount; ++i) {
        int wx, wy, worient;
        inFile >> wx >> wy >> worient;
        
        Wall loadedWall;
        loadedWall.topLeft = {wx, wy};
        loadedWall.orientation = static_cast<WallOrientation>(worient);
        
        board.placeWall(loadedWall); // Directly push into data vector
    }

    inFile.close();
    return true;
}
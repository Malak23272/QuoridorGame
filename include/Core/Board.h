#pragma once
#include <vector>
#include "Types.h"
#include "Player.h"
#include <queue>

// Encapsulates a decision made by the AI
struct Move {
    bool isWallPlacement;
    Position pawnMove;   // Used if isWallPlacement is false
    Wall wallToPlace;    // Used if isWallPlacement is true
    int evaluationScore; // Internal score used by the Minimax algorithm
};

class Board {
private:
    static const int BOARD_SIZE = 9;
    std::vector<Wall> placedWalls;
    
    // Helper to check if a specific edge is blocked by a wall
    bool isEdgeBlocked(Position from, Position to) const{
//swap to always consider right and down if it's left or up swap them 
//swap coordinates
if(to.x<from.x||to.y<from.y){
    std::swap(to,from);
}


    //check if the are side by side (horizontal)
    if((to.x==from.x+1)&&to.y==from.y){
    //check the walls vertical is vertical wall at my current position it'top left or the top left
    for(int i=0;i<placedWalls.size();++i){
        if(placedWalls[i].orientation==WallOrientation::VERTICAL){
        if((placedWalls[i].topLeft.x==from.x)&&(placedWalls[i].topLeft.y==from.y)) return true;
        else if((placedWalls[i].topLeft.x==from.x)&&(placedWalls[i].topLeft.y==from.y-1)) return true;
    }
    
    }

    }
     else if((to.x==from.x)&&to.y==from.y+1){
    //check the walls horizontal is there horizontal wall in my current position or the one before my current 
    for(int i=0;i<placedWalls.size();++i){
        if(placedWalls[i].orientation==WallOrientation::HORIZONTAL){
        if((placedWalls[i].topLeft.x==from.x)&&(placedWalls[i].topLeft.y==from.y)) return true;
        else if((placedWalls[i].topLeft.x==from.x-1)&&(placedWalls[i].topLeft.y==from.y)) return true;
    }}}
    return false;
};


//helper function to check whether does this player have path to GOAL?
bool hasPath_to_Goal(Player player){

//create 9*9 board and assume the player hasn't visited anyposition yet

bool visited[9][9]={false};//all the indexes are false

//create queue of the positions to check about the path
std::queue<Position>positions;

//in each current position it will need to check the 4 neighbors (up ,down,left,right)
//so we have 2 options handle each case separetly or make 4 mini arrays for the change in x and change in y
int dx[4]={0,0,-1,1};
int dy[4]={-1,1,0,0};


//first add the player's current position in the queue
positions.push(player.getPosition());//returns player current position 

visited[player.getPosition().x][player.getPosition().y]=true;//set it as it's visited 

//while loop to check on the whole positions
while(!positions.empty()){
     Position pos=positions.front();//grab the position from queue
     positions.pop();//remove it from queue

     if(pos.y==player.getGoalRow())
     {//if yes so The player found the Goal 
              return true;    
    }

     //this position needs to check the 4 neighbors
     //check the 4 neighbors automatically
     for(int i=0;i<4;++i){
        Position p;
    //this way we handle the 4 neighbors
     p.x=pos.x+dx[i];
     p.y=pos.y+dy[i];

     //to add the position in the queue and considered it with us
     //this position should be not visited 
     //this position shouldn't be out of bounds
    //this position shouldn't be wall blocked
     if((p.x<9&&p.y<9&&p.y>=0&&p.x>=0)&&(visited[p.x][p.y]==false)&&!(isEdgeBlocked(pos,p))){
        positions.push(p);//push this position in the queue
        visited[p.x][p.y]=true;//mark it as visited
     }}}
     //if we exist from while loop and we can't find goal return false
     return false;
    };


public:
    Board();

    // Validates if a pawn move is legal (orthogonal, no walls blocking, valid jumps)
    bool isValidPawnMove(Position current, Position target, Position opponentPos) const;
    
    // Validates if a wall overlaps another wall or goes out of bounds
    bool isValidWallPlacement(Wall wall) const;
    
    // This is where competitive programming experience will shine.
    // It requires a classic graph traversal (BFS or DFS) to guarantee 
    // that placing this new wall doesn't trap either player.
    bool doesWallBlockPath(Wall newWall, const Player& p1, const Player& p2) ;

    // Applies the wall to the board
    void placeWall(Wall wall);

    //to get the shortest path for the player
    int getShortestPath( Player& player);   
         
         
    
    // Getters for the UI (to draw them) and AI (to evaluate the board)
    const std::vector<Wall>& getWalls() const;
    int getSize() const { return BOARD_SIZE; }
    void clearBoard(){ placedWalls.clear(); }
};


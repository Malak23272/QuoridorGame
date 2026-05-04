#include<iostream>
#include <vector>
#include "C:\computer programming\QuoridorProject\include\Core\Board.h"
using namespace std;

Board::Board(){}

bool Board::isValidPawnMove(Position current, Position target, Position opponentPos) const {
    //if it's out of bound
    if(target.x>=9||target.y>=9||target.y<0||target.x<0) return false;
    //if the target is the same oppnonent position
    if(target==opponentPos) return false;
    //FIRST WE NEED TO know is it 1 step or 2 step (JUMP) or diagonally?
    int step_x=std::abs(target.x-current.x);
    int step_y=std::abs(target.y-current.y);
    int total_step=step_x+step_y;

    if(total_step==1){
        //it's just 1 step
        //so here I need to check if there is blocking wall?
        //if yes it will return true in isedge block and in isvalid pawn return false
        return !isEdgeBlocked(current,target);
    }
    //if total step not =1 we need to check does opponent one step from current
    //because if yes we will check the jump 

    int curr_opp_distance=std::abs(opponentPos.x-current.x)+std::abs(opponentPos.y-current.y);
    
    //if not =1 that means they aren't next to each other 
    //that means it's invalid move to move more than 1 step opponent should be next to current
    if(curr_opp_distance!=1)return false;//invalid movement

    //if they are next to each other but they have wall it can't move 2 steps 
    if(isEdgeBlocked(current,opponentPos)) return false;

    //WE NEED HERE TO DISCUSS THE MOVEMENT JUMP
    //BRING THE POSITION BEHIND OPPONENT
    Position behind;
    behind.x=opponentPos.x+std::abs(opponentPos.x-current.x);//get behind in any case left ,right,up,down
    behind.y=opponentPos.y+std::abs(opponentPos.y-current.y);//the same here as well

    //what if the behind position is out of bounds?
    bool behind_out_of_bounds=(behind.x>=9||behind.y>=9||behind.y<0||behind.x<0)?1:0;

    //what if it's wall blocked
    bool invalid_jump=(behind_out_of_bounds||isEdgeBlocked(opponentPos,behind))?1:0;
    
    //so here check the target =behind
    if(target==behind){
     //check behind case through invalid jump
     if(invalid_jump) return false;//can't jump here
     return true; }
     
     //check the diagonal movement
     //actually this movement is changing x and y together unlike any other one
     if(step_x==1&&step_y==1){
     
        if(!invalid_jump){
            return false;
            //why because if it's valid jump you can't move diagonally 
        }

        //check that the target moving diagonally forwards not backwards
        int check_movement_of_diagonal=std::abs(target.x-opponentPos.x)+std::abs(target.y-opponentPos.y);

        //if this movement not =1 
        //that means it tries to move diagonally but backwards
        //if it =1 that means it moves diagonally forwards
        //if it's =1 yes okay but i need to check the wall blocking again
        if(check_movement_of_diagonal==1) {
            return !isEdgeBlocked(opponentPos,target);//if it's edge block can't move in this diagonal
        } }

        //else it's valid move so return true
        return true;}


bool Board:: doesWallBlockPath(Wall newWall, const Player& p1, const Player& p2) {
//add the wall in the placed walls vector
placedWalls.push_back(newWall);
//if both are true that means both can find path and this wall doesn't block path 
bool find_goal_for_both=hasPath_to_Goal(p1)&&hasPath_to_Goal(p2);

//remove the wall from vector it was tempoary just for checking 
//it just hypothesis that BFS always ask what if i add the wall here will it block someone?
//not actual placement for the wall
placedWalls.pop_back();

return !find_goal_for_both;//if either  is blocked it will return true,if both not blocked it will return false
}

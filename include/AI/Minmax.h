// AI/Minimax.h
#pragma once
#include "../Core/Board.h"
#include "../Core/Player.h"
#include <vector>

class Minimax {
private:
    int maxSearchDepth;
    PlayerId aiPlayerId;
    Move BestMove;

    // The heuristic function: This is the "secret sauce" of your AI.
    // It should calculate: (Opponent's shortest path to goal) - (AI's shortest path to goal)
    int evaluateBoardState(Board& board,  Player& aiPlayer,  Player& opponent) const;

    // Generates all legal pawn moves AND valid wall placements for a given state
    std::vector<Move> generateAllLegalMoves(Board& board, const Player& activePlayer,const Player &opponnentPlayer) const;

    // The recursive alpha-beta pruning function
    int minimaxRecursive(Board currentBoard, Player currentPlayer, Player opponentPlayer, 
                         int depth, int alpha, int beta, bool isMaximizing);
                         //function to help creating a move and test it
         void applymove(const Move &m,Board &b,Player &p);

public:
    Minimax(int depth, PlayerId id):maxSearchDepth(depth),aiPlayerId(id){}
};
 void Minimax::applymove(const Move &m,Board&b,Player &p){
             if(m.isWallPlacement){
                   b.placeWall(m.wallToPlace);
                   p.useWall();
                }

                else{
                    
                      p.setPosition(m.pawnMove);
                       
                } }
            

int Minimax::evaluateBoardState( Board& board,  Player& aiPlayer,  Player& opponent) const{
    return board.getShortestPath(opponent)-board.getShortestPath(aiPlayer);}
    //////////////////////////

int Minimax ::minimaxRecursive(Board currentBoard, Player currentPlayer, Player opponentPlayer, 
                         int depth, int alpha, int beta, bool isMaximizing){
                            if(depth==0) return evaluateBoardState(currentBoard,currentPlayer,opponentPlayer);
                            Player p;
                            std::vector<Move>moves;
                            if(isMaximizing){
                                p=currentPlayer;
                            moves=generateAllLegalMoves(currentBoard,p,opponentPlayer);}
                              else{
                                p=opponentPlayer;
                                moves=generateAllLegalMoves(currentBoard,p,currentPlayer);
                              }
                              
                            if(isMaximizing){
                                int max=-100000;
                                for(auto const &m:moves){
                                    Board tempBoard=currentBoard;
                                    Player tempCurrent = currentPlayer;
                                     Player tempOpponent = opponentPlayer;
                                    applymove(m,tempBoard,tempCurrent);
                                    int current_score=minimaxRecursive(tempBoard,tempCurrent,tempOpponent,depth-1,
                                    alpha,beta,!isMaximizing);
                                    if(current_score>max){
                                        max=current_score;
                                        if(depth==maxSearchDepth){
                                        BestMove=m;}
                                    }
                                alpha=std::max(alpha,current_score);
                                if(alpha>=beta){
                                    break;
                                }
                                }
                            return max;
                            }

                                    else{ int min=100000;
                                for(auto const &m:moves){
                                     Board tempBoard=currentBoard;
                                      Player tempCurrent = currentPlayer;
                                     Player tempOpponent = opponentPlayer;
                                    applymove(m,tempBoard,tempOpponent);

                                    int current_score=minimaxRecursive(tempBoard,tempCurrent,tempOpponent,depth-1,
                                    alpha,beta,!isMaximizing);
                                    if(current_score<min){
                                        min=current_score;
                                       
                                    }
                                
                            beta=std::min(beta,current_score);

                            if(alpha>=beta){
                                break;
                            }
                                }
                            return min;
                            }}


    std::vector<Move> Minimax:: generateAllLegalMoves(Board& board, const Player& activePlayer,const Player &opponnentPlayer) const{
       std::vector<Move>legalmoves;
       //pawns moves
    for(int dx=-2;dx<=2;++dx){
        for(int dy=-2;dy<=2;++dy){
        Position p;
        p.x=activePlayer.getPosition().x+dx;
        p.y=activePlayer.getPosition().y+dy;
        if(board.isValidPawnMove(activePlayer.getPosition(),p,opponnentPlayer.getPosition())){
            legalmoves.push_back({0,p});
        }
        }
    }
    //walls moves
    if(activePlayer.getWallsRemaining()>0){
    for(int x=0;x<=7;++x){
      for(int y=0;y<=7;++y){

    Wall hwall;
    hwall.orientation=WallOrientation::HORIZONTAL;
    hwall.topLeft.x=x;
    hwall.topLeft.y=y;
    if(board.isValidWallPlacement(hwall)&&!board.doesWallBlockPath(hwall,activePlayer,opponnentPlayer)){
        legalmoves.push_back({1,Position(),hwall});
    }
    Wall Vwall;
    Vwall.orientation=WallOrientation::VERTICAL;
    Vwall.topLeft.x=x;
    Vwall.topLeft.y=y;
    if(board.isValidWallPlacement(Vwall)&&!board.doesWallBlockPath(Vwall,activePlayer,opponnentPlayer)){
        legalmoves.push_back({1,Position(),Vwall});
    }}

      }}
      return legalmoves;
    }



                         
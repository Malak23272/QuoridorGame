// AI/Minimax.h
#pragma once
#include "../Core/Board.h"
#include "../Core/Player.h"
#include <vector>
#include <algorithm>

class AIPlayer;

class Minimax {
private:
    int maxSearchDepth;
    PlayerId aiPlayerId;
    Move BestMove;

    friend class AIPlayer ;

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
    Minimax();
    Minimax(int depth, PlayerId id):maxSearchDepth(depth),aiPlayerId(id){}

    Move getBestMove(){return BestMove;}
    Move calculateBestMove(Board currentBoard, Player currentPlayer, Player opponentPlayer);
};
 inline void Minimax::applymove(const Move &m,Board&b,Player &p){
             if(m.isWallPlacement){
                   b.placeWall(m.wallToPlace);
                   p.useWall();
                }

                else{
                    
                      p.setPosition(m.pawnMove);
                       
                } }
            

inline int Minimax::evaluateBoardState( Board& board,  Player& aiPlayer,  Player& opponent) const{
    // Base score: opponent path length minus AI path length (higher = better for AI)
    int score = board.getShortestPath(opponent)-board.getShortestPath(aiPlayer);
    // Small bonus for having more walls remaining (strategic flexibility)
    score += (aiPlayer.getWallsRemaining() - opponent.getWallsRemaining()) * 2;
    return score;}
    //////////////////////////

inline int Minimax ::minimaxRecursive(Board currentBoard, Player currentPlayer, Player opponentPlayer, 
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


    inline std::vector<Move> Minimax:: generateAllLegalMoves(Board& board, const Player& activePlayer,const Player &opponnentPlayer) const{
       std::vector<Move>legalmoves;
       //pawns moves — iterate dy in goal-direction order (forward first)
    int goalRow = activePlayer.getGoalRow();
    int fwd = (goalRow == 0) ? -1 : 1;
    for(int dy : {fwd, 2*fwd, 0, -fwd, -2*fwd}) {
        for(int dx=-2;dx<=2;++dx){
            Position p;
            p.x=activePlayer.getPosition().x+dx;
            p.y=activePlayer.getPosition().y+dy;
            if(board.isValidPawnMove(activePlayer.getPosition(),p,opponnentPlayer.getPosition())){
                legalmoves.push_back({0,p});
            }
        }
    }
    //walls moves — restrict to region between both players for performance + strategic relevance
    if(activePlayer.getWallsRemaining()>0){
    int px=activePlayer.getPosition().x, py=activePlayer.getPosition().y;
    int ox=opponnentPlayer.getPosition().x, oy=opponnentPlayer.getPosition().y;
    int x0=std::max(0,std::min(px,ox)-2), x1=std::min(7,std::max(px,ox)+2);
    int y0=std::max(0,std::min(py,oy)-2), y1=std::min(7,std::max(py,oy)+2);
    for(int x=x0;x<=x1;++x){
      for(int y=y0;y<=y1;++y){

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


inline Move Minimax::calculateBestMove(Board currentBoard, Player currentPlayer, Player opponentPlayer){
    // Initialize to invalid state; updated by search if any move found
    BestMove.isWallPlacement = false;
    BestMove.pawnMove = currentPlayer.getPosition();
    BestMove.wallToPlace = {};
    BestMove.evaluationScore = -100000;

    // First find ALL root-level legal moves (the search will refine)
    std::vector<Move> rootMoves = generateAllLegalMoves(currentBoard, currentPlayer, opponentPlayer);


    if (rootMoves.empty()) {
        // No legal moves at all — return a safe forward step
        Position p = currentPlayer.getPosition();
        int goalRow = currentPlayer.getGoalRow();
        int dy = (goalRow == 0) ? -1 : 1; // toward goal
        Position fwd{p.x, p.y + dy};
        if (currentBoard.isValidPawnMove(p, fwd, opponentPlayer.getPosition())) {
            BestMove = {false, fwd, {}, -100000};
        } else {
            // Try sides
            for (int dx : {-1, 1}) {
                Position side{p.x + dx, p.y};
                if (currentBoard.isValidPawnMove(p, side, opponentPlayer.getPosition())) {
                    BestMove = {false, side, {}, -100000};
                    break;
                }
            }
        }
        return BestMove;
    }

    minimaxRecursive(currentBoard, currentPlayer, opponentPlayer, maxSearchDepth, -100000, 100000, 1);

    // Safety: ensure BestMove is actually in rootMoves (defensive against edge cases)
    bool valid = false;
    for (const auto& rm : rootMoves) {
        if (rm.isWallPlacement == BestMove.isWallPlacement) {
            if (BestMove.isWallPlacement) {
                if (rm.wallToPlace.topLeft.x == BestMove.wallToPlace.topLeft.x &&
                    rm.wallToPlace.topLeft.y == BestMove.wallToPlace.topLeft.y &&
                    rm.wallToPlace.orientation == BestMove.wallToPlace.orientation) {
                    valid = true; break;
                }
            } else {
                if (rm.pawnMove.x == BestMove.pawnMove.x && rm.pawnMove.y == BestMove.pawnMove.y) {
                    valid = true; break;
                }
            }
        }
    }
    if (!valid) {
        BestMove = rootMoves[0];
    }
    return BestMove;
}
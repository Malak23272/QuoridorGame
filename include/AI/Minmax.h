// AI/Minimax.h
#pragma once
#include "../Core/Board.h"
#include "../Core/Player.h"
#include <algorithm>
#include <vector>

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
            

inline int Minimax::evaluateBoardState(Board& board, Player& aiPlayer, Player& opponent) const {
    // 1. ABSOLUTE WIN/LOSS
    if (opponent.getPosition().y == opponent.getGoalRow()) return -1000000;
    if (aiPlayer.getPosition().y == aiPlayer.getGoalRow()) return 1000000;

    // 2. The Distances
   int aiDist = board.getShortestPath(aiPlayer, opponent.getPosition());
int oppDist = board.getShortestPath(opponent, aiPlayer.getPosition());
    
    // ---------------------------------------------------------
    // THE FIX: Self-Preservation Weighting
    // We multiply AI distance by 20, but Opponent distance by 10.
    // The AI now cares TWICE as much about running to the finish line 
    // as it does about slowing you down!
    // ---------------------------------------------------------
    int mainScore = (oppDist * 10) - (aiDist * 20); 
    
    // 3. Center Bias (Tie-breaker)
    int centerBias = 4 - std::abs(aiPlayer.getPosition().x - 4);
    
    return mainScore + centerBias;
}
    //////////////////////////

 inline int Minimax ::minimaxRecursive(Board currentBoard, Player currentPlayer, Player opponentPlayer, 
                         int depth, int alpha, int beta, bool isMaximizing){
             // Stop the Time Machine if depth is 0 OR if someone wins!
    if (depth == 0 || currentPlayer.getPosition().y == currentPlayer.getGoalRow() || opponentPlayer.getPosition().y == opponentPlayer.getGoalRow()) {
        
        int score = evaluateBoardState(currentBoard, currentPlayer, opponentPlayer);
        
        // SURVIVAL MECHANIC: Add the depth to the score. 
        // A loss at Depth 3 (-999997) is mathematically better than a loss at Depth 1 (-999999). 
        // This forces the AI to place blocking walls to delay its death!
        if (score <= -900000) return score + depth; 
        if (score >= 900000) return score + depth;
        
        return score;
    }
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


   inline std::vector<Move> Minimax::generateAllLegalMoves(Board& board, const Player& activePlayer, const Player &opponnentPlayer) const {
    std::vector<Move> legalmoves;

    int goalDir = (activePlayer.getGoalRow() == 0) ? -1 : 1;
    int px = activePlayer.getPosition().x;
    int py = activePlayer.getPosition().y;

    for (int dx = -2; dx <= 2; ++dx) {
        for (int dy = -2; dy <= 2; ++dy) {
            Position p;
            p.x = px + dx;
            p.y = py + dy;
            if (board.isValidPawnMove(activePlayer.getPosition(), p, opponnentPlayer.getPosition())) {
                int score = dy * goalDir;
                legalmoves.push_back({false, p, Wall(), score});
            }
        }
    }

    if (activePlayer.getWallsRemaining() > 0) {
        int ox = opponnentPlayer.getPosition().x;
        int oy = opponnentPlayer.getPosition().y;
        for (int x = 0; x <= 7; ++x) {
            for (int y = 0; y <= 7; ++y) {
                int distToActive = std::abs(px - x) + std::abs(py - y);
                int distToOpp = std::abs(ox - x) + std::abs(oy - y);
                if (distToActive > 3 && distToOpp > 3) continue;

                Wall hwall;
                hwall.orientation = WallOrientation::HORIZONTAL;
                hwall.topLeft.x = x;
                hwall.topLeft.y = y;
                if (board.isValidWallPlacement(hwall) && !board.doesWallBlockPath(hwall, activePlayer, opponnentPlayer)) {
                    int score = distToActive - distToOpp;
                    legalmoves.push_back({true, Position(), hwall, score});
                }

                Wall vwall;
                vwall.orientation = WallOrientation::VERTICAL;
                vwall.topLeft.x = x;
                vwall.topLeft.y = y;
                if (board.isValidWallPlacement(vwall) && !board.doesWallBlockPath(vwall, activePlayer, opponnentPlayer)) {
                    int score = distToActive - distToOpp;
                    legalmoves.push_back({true, Position(), vwall, score});
                }
            }
        }
    }

    std::sort(legalmoves.begin(), legalmoves.end(), [](const Move& a, const Move& b) {
        int rankA = a.isWallPlacement ? (a.evaluationScore - 100) : (a.evaluationScore + 100);
        int rankB = b.isWallPlacement ? (b.evaluationScore - 100) : (b.evaluationScore + 100);
        return rankA > rankB;
    });

    return legalmoves;
    }


 inline Move Minimax::calculateBestMove(Board currentBoard, Player currentPlayer, Player opponentPlayer){
    std::vector<Move> initialMoves = generateAllLegalMoves(currentBoard, currentPlayer, opponentPlayer);
    if (!initialMoves.empty()) {
        BestMove = initialMoves[0]; 
    }
    minimaxRecursive(currentBoard, currentPlayer, opponentPlayer, maxSearchDepth, -100000, 100000, 1);
    return BestMove;
}
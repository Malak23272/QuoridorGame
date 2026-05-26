#include "Board.h"

#include <iostream>
#include <vector>
using namespace std;

Board::Board() {}

bool Board::isValidPawnMove(Position current, Position target, Position opponentPos) const {
    // if it's out of bound
    if (target.x >= 9 || target.y >= 9 || target.y < 0 || target.x < 0) return false;
    // if the target is the same oppnonent position
    if (target == opponentPos) return false;
    // FIRST WE NEED TO know is it 1 step or 2 step (JUMP) or diagonally?
    int step_x = std::abs(target.x - current.x);
    int step_y = std::abs(target.y - current.y);
    int total_step = step_x + step_y;

    if (total_step == 1) {
        // it's just 1 step
        // so here I need to check if there is blocking wall?
        // if yes it will return true in isedge block and in isvalid pawn return false
        return !isEdgeBlocked(current, target);
    }
    // if total step not =1 we need to check does opponent one step from current
    // because if yes we will check the jump

    int curr_opp_distance = std::abs(opponentPos.x - current.x) + std::abs(opponentPos.y - current.y);

    // if not =1 that means they aren't next to each other
    // that means it's invalid move to move more than 1 step opponent should be next to current
    if (curr_opp_distance != 1) return false;  // invalid movement

    // if they are next to each other but they have wall it can't move 2 steps
    if (isEdgeBlocked(current, opponentPos)) return false;

    // The cell on the far side of the opponent (used for jumping over)
    Position behind;
    behind.x = opponentPos.x + (opponentPos.x - current.x);
    behind.y = opponentPos.y + (opponentPos.y - current.y);

    bool behind_out_of_bounds = (behind.x >= 9 || behind.y >= 9 || behind.y < 0 || behind.x < 0);
    bool invalid_jump = behind_out_of_bounds || isEdgeBlocked(opponentPos, behind);

    // Jump over opponent (2 steps in the same direction)
    if (target == behind) {
        if (invalid_jump) return false;
        return true;
    }

    // Diagonal escape (only allowed when the jump is blocked)
    if (step_x == 1 && step_y == 1) {
        if (!invalid_jump) return false;
        int diag_dist = std::abs(target.x - opponentPos.x) + std::abs(target.y - opponentPos.y);
        if (diag_dist == 1) {
            return !isEdgeBlocked(opponentPos, target);
        }
    }

    return false;
}

bool Board::doesWallBlockPath(Wall newWall, const Player& p1, const Player& p2) {
    // add the wall in the placed walls vector
    placedWalls.push_back(newWall);
    // if both are true that means both can find path and this wall doesn't block path
    bool find_goal_for_both = hasPath_to_Goal(p1) && hasPath_to_Goal(p2);

    // remove the wall from vector it was tempoary just for checking
    // it just hypothesis that BFS always ask what if i add the wall here will it block someone?
    // not actual placement for the wall
    placedWalls.pop_back();

    return !find_goal_for_both;  // if either  is blocked it will return true,if both not blocked it will return false
}

bool Board::isValidWallPlacement(Wall wall) const {
    // case1: wall out of bounds
    if (wall.topLeft.x < 0 || wall.topLeft.y < 0) return false;
    if (wall.topLeft.x >= 8 || wall.topLeft.y >= 8) return false;

    // case2: walls overlaps
    for (int i = 0; i < placedWalls.size(); i++) {
        if (placedWalls[i].topLeft == wall.topLeft) return false;

        if (wall.orientation == placedWalls[i].orientation) {
            if (wall.orientation == WallOrientation::HORIZONTAL && placedWalls[i].topLeft.x == wall.topLeft.x + 1 &&
                placedWalls[i].topLeft.y == wall.topLeft.y)
                return false;
            if (wall.orientation == WallOrientation::HORIZONTAL && placedWalls[i].topLeft.x == wall.topLeft.x - 1 &&
                placedWalls[i].topLeft.y == wall.topLeft.y)
                return false;

            if (wall.orientation == WallOrientation::VERTICAL && placedWalls[i].topLeft.y == wall.topLeft.y + 1 &&
                placedWalls[i].topLeft.x == wall.topLeft.x)
                return false;
            if (wall.orientation == WallOrientation::VERTICAL && placedWalls[i].topLeft.y == wall.topLeft.y - 1 &&
                placedWalls[i].topLeft.x == wall.topLeft.x)
                return false;
        }

        if (wall.topLeft.x == placedWalls[i].topLeft.x && wall.topLeft.y == placedWalls[i].topLeft.y) return false;
    }

    return true;
}

void Board::placeWall(Wall wall) {
    placedWalls.push_back(wall);
}

const std::vector<Wall>& Board::getWalls() const {
    return placedWalls;
}

int Board::getShortestPath(Player& player) {
    int distance[9][9];
    queue<Position> positions;

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            distance[i][j] = -1;
        }
    }
    // up down left right
    int dx[4] = {0, 0, -1, 1};
    int dy[4] = {-1, 1, 0, 0};
    positions.push(player.getPosition());
    distance[player.getPosition().x][player.getPosition().y] = 0;

    while (!positions.empty()) {
        Position pos = positions.front();
        positions.pop();
        if (pos.y == player.getGoalRow()) {
            return distance[pos.x][pos.y];
        }
        for (int i = 0; i < 4; ++i) {
            Position p;
            p.x = pos.x + dx[i];
            p.y = pos.y + dy[i];

            if (p.x >= 0 && p.x < 9 && p.y < 9 && p.y >= 0) {
                if (distance[p.x][p.y] == -1 && !isEdgeBlocked(pos, p)) {
                    positions.push(p);

                    distance[p.x][p.y] = distance[pos.x][pos.y] + 1;  // to increment the distance
                }
            }
        }
    }
    return 999;  // invalid or trigger for AI to not trying use this path
}

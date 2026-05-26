// Quick automated test of core game logic
#include <cassert>
#include <cstdio>
#include <string>

#include "../include/AI/AIPlayer.h"
#include "../include/AI/Minmax.h"
#include "../include/Core/Board.h"
#include "../include/Core/GameEngine.h"
#include "../include/Core/Player.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name)                            \
    do {                                      \
        std::printf("  TEST: %s ... ", name); \
        tests_run++;                          \
    } while (0)
#define PASS()                 \
    do {                       \
        std::printf("PASS\n"); \
        tests_passed++;        \
    } while (0)
#define FAIL(msg)                       \
    do {                                \
        std::printf("FAIL: %s\n", msg); \
    } while (0)

static void test_initial_state() {
    GameEngine engine;
    assert(!engine.isGameOver());
    assert(engine.getCurrentTurn() == PlayerId::PLAYER_1);
    assert(engine.getPlayer1().getPosition().x == 4);
    assert(engine.getPlayer1().getPosition().y == 8);
    assert(engine.getPlayer2().getPosition().x == 4);
    assert(engine.getPlayer2().getPosition().y == 0);
    assert(engine.getPlayer1().getWallsRemaining() == 10);
    assert(engine.getPlayer2().getWallsRemaining() == 10);
}

static void test_pawn_move_basic() {
    GameEngine engine;
    // Player 1 moves up (one step)
    assert(engine.movePawn({4, 7}));
    assert(engine.getCurrentTurn() == PlayerId::PLAYER_2);
    assert(engine.getPlayer1().getPosition().y == 7);
    // Player 2 moves down
    assert(engine.movePawn({4, 1}));
    assert(engine.getCurrentTurn() == PlayerId::PLAYER_1);
    assert(engine.getPlayer2().getPosition().y == 1);
}

static void test_pawn_move_invalid() {
    GameEngine engine;
    // Can't move out of bounds
    assert(!engine.movePawn({-1, 8}));
    assert(!engine.movePawn({4, -1}));
    assert(!engine.movePawn({9, 8}));
    // Can't move 2 steps when opponent not adjacent
    assert(!engine.movePawn({4, 6}));
    // Can't stay in place
    assert(!engine.movePawn({4, 8}));
}

static void test_wall_placement() {
    GameEngine engine;
    // P1 places a horizontal wall
    Wall w;
    w.topLeft = {0, 0};
    w.orientation = WallOrientation::HORIZONTAL;
    assert(engine.placeWall(w));
    assert(engine.getPlayer1().getWallsRemaining() == 9);
    assert(engine.getCurrentTurn() == PlayerId::PLAYER_2);
}

static void test_wall_placement_invalid() {
    GameEngine engine;
    // Wall out of bounds
    Wall w;
    w.topLeft = {8, 8};
    w.orientation = WallOrientation::HORIZONTAL;
    assert(!engine.placeWall(w));
    // Duplicate wall
    w.topLeft = {0, 0};
    assert(engine.placeWall(w));
    w.topLeft = {0, 0};
    assert(!engine.placeWall(w));  // should be P2's turn now
}

static void test_save_load() {
    // Setup a known game state
    GameEngine engine;
    engine.movePawn({4, 7});  // P1
    engine.movePawn({4, 1});  // P2
    Wall w;
    w.topLeft = {2, 3};
    w.orientation = WallOrientation::HORIZONTAL;
    engine.placeWall(w);  // P1

    std::string savefile = "save_test.txt";
    assert(engine.saveGame(savefile));

    // Load into fresh engine
    GameEngine loaded;
    assert(loaded.loadGame(savefile));

    // Verify identical state
    assert(loaded.getCurrentTurn() == engine.getCurrentTurn());
    assert(loaded.getPlayer1().getPosition().x == engine.getPlayer1().getPosition().x);
    assert(loaded.getPlayer1().getPosition().y == engine.getPlayer1().getPosition().y);
    assert(loaded.getPlayer1().getWallsRemaining() == engine.getPlayer1().getWallsRemaining());
    assert(loaded.getPlayer2().getPosition().x == engine.getPlayer2().getPosition().x);
    assert(loaded.getPlayer2().getPosition().y == engine.getPlayer2().getPosition().y);
    assert(loaded.getPlayer2().getWallsRemaining() == engine.getPlayer2().getWallsRemaining());
    assert(loaded.getBoard().getWalls().size() == engine.getBoard().getWalls().size());

    // Clean up
    std::remove(savefile.c_str());
}

static void test_pawn_jump() {
    GameEngine engine;
    // Move pawns adjacent for jump test
    engine.movePawn({4, 7});  // P1 up 1
    engine.movePawn({4, 1});  // P2 down 1
    engine.movePawn({4, 6});  // P1 up 1
    engine.movePawn({4, 2});  // P2 down 1
    engine.movePawn({4, 5});  // P1 up 1
    engine.movePawn({4, 3});  // P2 down 1
    // Now P1 at (4,5), P2 at (4,3) - they're 2 apart, not adjacent yet
    engine.movePawn({4, 4});  // P1 up 1 -> now at (4,4), P2 at (4,3) - adjacent!
    // P2's turn - P2 jumps over P1
    assert(engine.movePawn({4, 5}));  // Jump over P1 from (4,3) to (4,5)
    assert(engine.getPlayer2().getPosition().y == 5);
}

static void test_ai_easy_makes_valid_move() {
    // Test that AI at Easy depth can make a valid move
    Minimax brain(1, PlayerId::PLAYER_2);
    Board board;
    Player aiPlayer(PlayerId::PLAYER_2);
    Player opponent(PlayerId::PLAYER_1);

    Move best = brain.calculateBestMove(board, aiPlayer, opponent);

    // AI should return SOME move
    assert(best.isWallPlacement || best.pawnMove.x >= 0);
}

static void test_move_invalid_no_move() {
    GameEngine engine;
    // engine should prevent invalid moves, no crash
    assert(!engine.movePawn({4, 4}));  // Can't move diagonally without adjacency
}

static void test_blocked_jump() {
    GameEngine engine;
    // Place P1 at (4,4), P2 at (4,5) vertically adjacent
    // Move P1 to (4,4)
    engine.movePawn({4, 7});
    engine.movePawn({4, 1});
    engine.movePawn({4, 6});
    engine.movePawn({4, 2});
    engine.movePawn({4, 5});
    engine.movePawn({4, 3});
    engine.movePawn({4, 4});
    // Now P1 at (4,4), P2 at (4,3) - 1 apart
    // P2 tries to jump to (4,5) - this should be valid since no wall blocks
    assert(engine.movePawn({4, 5}));  // jump over P1 at (4,4)
    assert(engine.getPlayer2().getPosition().y == 5);
}

static void test_winner() {
    GameEngine engine;
    // P1 moves up column 4, P2 moves down column 4.
    // When P2 reaches y=8 (its goal), P2 wins.
    engine.movePawn({4, 7});  // P1 (4,8)->(4,7)
    engine.movePawn({4, 1});  // P2 (4,0)->(4,1)
    engine.movePawn({4, 6});  // P1 (4,7)->(4,6)
    engine.movePawn({4, 2});  // P2 (4,1)->(4,2)
    engine.movePawn({4, 5});  // P1 (4,6)->(4,5)
    engine.movePawn({4, 3});  // P2 (4,2)->(4,3)
    engine.movePawn({4, 4});  // P1 (4,5)->(4,4)

    // P2 at (4,3), P1 at (4,4) adjacent. P2 jumps over P1 to (4,5)
    engine.movePawn({4, 5});  // P2 jumps over P1 to (4,5)

    engine.movePawn({4, 3});  // P1 (4,4)->(4,3)
    engine.movePawn({4, 6});  // P2 (4,5)->(4,6)
    engine.movePawn({4, 2});  // P1 (4,3)->(4,2)
    engine.movePawn({4, 7});  // P2 (4,6)->(4,7)
    engine.movePawn({4, 1});  // P1 (4,2)->(4,1)
    engine.movePawn({4, 8});  // P2 (4,7)->(4,8) wins!

    assert(engine.getPlayer2().getPosition().y == 8);
    assert(engine.isGameOver());
    assert(engine.getWinner() == PlayerId::PLAYER_2);
}

static void test_ai_plays_several_turns_without_deadlock() {
    // Simulate a real game with AI: P1 human-like moves, P2 is AI at Easy (depth 1)
    GameEngine engine;
    AIPlayer ai(PlayerId::PLAYER_2, AIDifficulty::EASY);

    // Play up to 100 half-turns; must not deadlock
    for (int turn = 0; turn < 100; ++turn) {
        if (engine.isGameOver()) break;

        if (engine.getCurrentTurn() == PlayerId::PLAYER_1) {
            // P1 makes a simple forward move toward goal (row 0)
            Position p = engine.getPlayer1().getPosition();
            Position fwd{p.x, p.y - 1};
            if (engine.getBoard().isValidPawnMove(p, fwd, engine.getPlayer2().getPosition())) {
                bool ok = engine.movePawn(fwd);
                assert(ok);
            } else {
                // Try sides if forward is blocked
                bool moved = false;
                for (int dx : {-1, 1}) {
                    Position side{p.x + dx, p.y};
                    if (engine.getBoard().isValidPawnMove(p, side, engine.getPlayer2().getPosition())) {
                        engine.movePawn(side);
                        moved = true;
                        break;
                    }
                }
                // If truly stuck, place a wall
                if (!moved && engine.getPlayer1().getWallsRemaining() > 0) {
                    Wall w;
                    w.orientation = WallOrientation::HORIZONTAL;
                    for (int x = 0; x < 7 && !moved; ++x)
                        for (int y = 0; y < 7 && !moved; ++y) {
                            w.topLeft = {x, y};
                            if (engine.placeWall(w)) moved = true;
                        }
                }
                assert(moved && "P1 must have a legal move");
            }
        } else {
            // AI takes its turn
            ai.takeTurn(engine);
        }
    }
}

static void test_ai_medium_makes_valid_move() {
    // Verify MEDIUM (depth 2) AI also returns a valid move
    Minimax brain(2, PlayerId::PLAYER_2);
    Board board;
    Player aiPlayer(PlayerId::PLAYER_2);
    Player opponent(PlayerId::PLAYER_1);
    Move best = brain.calculateBestMove(board, aiPlayer, opponent);
    assert(best.isWallPlacement || best.pawnMove.x >= 0);
}

static void test_ai_plays_several_turns_medium() {
    // Same as easy test but with MEDIUM difficulty (depth 2)
    GameEngine engine;
    AIPlayer ai(PlayerId::PLAYER_2, AIDifficulty::MEDIUM);

    // Play up to 100 half-turns; must not deadlock
    for (int turn = 0; turn < 100; ++turn) {
        if (engine.isGameOver()) break;

        if (engine.getCurrentTurn() == PlayerId::PLAYER_1) {
            // P1 makes a simple forward move toward goal (row 0)
            Position p = engine.getPlayer1().getPosition();
            Position fwd{p.x, p.y - 1};
            if (engine.getBoard().isValidPawnMove(p, fwd, engine.getPlayer2().getPosition())) {
                bool ok = engine.movePawn(fwd);
                assert(ok);
            } else {
                // Try sides if forward is blocked
                bool moved = false;
                for (int dx : {-1, 1}) {
                    Position side{p.x + dx, p.y};
                    if (engine.getBoard().isValidPawnMove(p, side, engine.getPlayer2().getPosition())) {
                        engine.movePawn(side);
                        moved = true;
                        break;
                    }
                }
                // If truly stuck, place a wall
                if (!moved && engine.getPlayer1().getWallsRemaining() > 0) {
                    Wall w;
                    w.orientation = WallOrientation::HORIZONTAL;
                    for (int x = 0; x < 7 && !moved; ++x)
                        for (int y = 0; y < 7 && !moved; ++y) {
                            w.topLeft = {x, y};
                            if (engine.placeWall(w)) moved = true;
                        }
                }
                assert(moved && "P1 must have a legal move");
            }
        } else {
            // AI takes its turn
            ai.takeTurn(engine);
        }
    }

    // Either the game ended with a winner, or it reached 100 turns without crash
    // (In practice on a 9x9 board with simple play it should finish)
}

static void test_diagnose_ai_start_move() {
    Board board;

    // Scenario 1: AI=P2, P1 moved forward (4,8)->(4,7), AI at (4,0)
    {
        Player ai(PlayerId::PLAYER_2);
        Player opp(PlayerId::PLAYER_1);
        opp.setPosition({4, 7});
        Minimax brain(2, PlayerId::PLAYER_2);
        Move best = brain.calculateBestMove(board, ai, opp);
        std::printf("  [P2 AI, after P1 forward]: pawnMove=(%d,%d), isWall=%d\n", best.pawnMove.x, best.pawnMove.y,
                    best.isWallPlacement);
    }

    // Scenario 2: AI=P1 (goes FIRST), P2 hasn't moved yet, AI at (4,8)
    {
        Player ai(PlayerId::PLAYER_1);
        Player opp(PlayerId::PLAYER_2);
        Minimax brain(2, PlayerId::PLAYER_1);
        Move best = brain.calculateBestMove(board, ai, opp);
        std::printf("  [P1 AI, first turn]:       pawnMove=(%d,%d), isWall=%d\n", best.pawnMove.x, best.pawnMove.y,
                    best.isWallPlacement);
    }

    // Scenario 3: AI=P2, P1 placed a wall instead of moving (still at (4,8))
    {
        Player ai(PlayerId::PLAYER_2);
        Player opp(PlayerId::PLAYER_1);
        // P1 used a wall, still at (4,8)
        opp.useWall();  // 9 walls left
        Board b2;
        Minimax brain(2, PlayerId::PLAYER_2);
        Move best = brain.calculateBestMove(b2, ai, opp);
        std::printf("  [P2 AI, after P1 wall]:    pawnMove=(%d,%d), isWall=%d\n", best.pawnMove.x, best.pawnMove.y,
                    best.isWallPlacement);
    }

    // Scenario 4: AI=P2, both have moved forward several steps
    {
        Player ai(PlayerId::PLAYER_2);
        Player opp(PlayerId::PLAYER_1);
        ai.setPosition({4, 3});   // AI has advanced
        opp.setPosition({4, 5});  // Opponent has advanced
        Minimax brain(2, PlayerId::PLAYER_2);
        Move best = brain.calculateBestMove(board, ai, opp);
        std::printf("  [P2 AI, mid-board]:        pawnMove=(%d,%d), isWall=%d\n", best.pawnMove.x, best.pawnMove.y,
                    best.isWallPlacement);
    }

    // Scenario 5: AI=P2, both at (4,6) and (4,2) — AI closer to opponent
    {
        Player ai(PlayerId::PLAYER_2);
        Player opp(PlayerId::PLAYER_1);
        ai.setPosition({4, 2});
        opp.setPosition({4, 6});
        Minimax brain(2, PlayerId::PLAYER_2);
        Move best = brain.calculateBestMove(board, ai, opp);
        std::printf("  [P2 AI, close to opp]:     pawnMove=(%d,%d), isWall=%d\n", best.pawnMove.x, best.pawnMove.y,
                    best.isWallPlacement);
    }
}

int main() {
    std::printf("Core game logic tests:\n");

    TEST("initial state");
    test_initial_state();
    PASS();
    TEST("pawn move basic");
    test_pawn_move_basic();
    PASS();
    TEST("pawn move invalid");
    test_pawn_move_invalid();
    PASS();
    TEST("wall placement");
    test_wall_placement();
    PASS();
    TEST("wall placement invalid");
    test_wall_placement_invalid();
    PASS();
    TEST("save and load");
    test_save_load();
    PASS();
    TEST("pawn jump");
    test_pawn_jump();
    PASS();
    TEST("AI easy valid move");
    test_ai_easy_makes_valid_move();
    PASS();
    TEST("invalid no move");
    test_move_invalid_no_move();
    PASS();
    TEST("blocked jump");
    test_blocked_jump();
    PASS();
    TEST("winner detection");
    test_winner();
    PASS();
    TEST("AI plays turns");
    test_ai_plays_several_turns_without_deadlock();
    PASS();
    TEST("AI medium valid move");
    test_ai_medium_makes_valid_move();
    PASS();
    TEST("AI medium plays turns");
    test_ai_plays_several_turns_medium();
    PASS();
    TEST("AI start move diag");
    test_diagnose_ai_start_move();
    PASS();

    std::printf("\n%d / %d tests passed.\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}

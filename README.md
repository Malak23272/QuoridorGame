# Quoridor Game

A desktop implementation of the classic **Quoridor** board game built with **Qt 6** and **C++20**. Features a minimax AI with alpha-beta pruning across three difficulty levels.

## Game Description

Quoridor is a two-player strategy game played on a 9×9 grid. Each player starts at opposite ends of the board and races to reach the other side. On each turn, a player may either:

- **Move their pawn** one step orthogonally (or jump over the opponent when adjacent), or
- **Place a wall** to block the opponent's path (each player has 10 walls).

The first player to reach any square on the opponent's starting row wins.

**AI Difficulty Levels:**
| Level | Search Depth | Description |
|-------|-------------|-------------|
| Easy   | 1 ply       | Greedy — picks the move that looks best immediately |
| Medium | 2 plies     | Sees one full turn ahead (AI move + opponent response) |
| Hard   | 4 plies     | Deep search — significantly stronger but slower |

## Screenshots

*Drag & drop screenshots here in the GitHub web editor.*

## Demo Video

[Watch the demo](https://youtu.be/your-video-id)

## Controls

- **Click a highlighted cell** — move your pawn to that position (adjacent cells are highlighted)
- **Click a wall gap** (the space between cells) — places a wall there
- **Wall orientation** — walls are placed horizontally or vertically depending on the gap orientation near your click
- **When hovering** over a valid wall position, a preview shows where the wall will be placed (green = valid, red = blocks a player)
- **Game menu** — New Game, Save Game, Load Game, Exit
- **Mode selector** — switch between Human vs Human and Human vs AI
- **Difficulty selector** — Easy / Medium / Hard

## Installation

### Requirements

- **Qt 6.11+** (Widgets and Concurrent modules)
- **MinGW-w64** (e.g., from MSYS2 UCRT64)
- **CMake 3.16+**
- **C++20** compiler (GCC 13+ or Clang 16+)

### Building from Source

```powershell
# Clone the repository
git clone https://github.com/Malak23272/QuoridorGame.git
cd QuoridorGame

# Run the build script (adjust Qt path in build.ps1 if needed)
.\build.ps1

# Or build manually:
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="path/to/g++" -S . -B build
mingw32-make -C build
```

### Running

```powershell
.\build\QuoridorGame.exe
```

If Qt DLLs are missing, deploy them with:

```powershell
windeployqt6.exe --no-translations build\QuoridorGame.exe
```

## Project Structure

```
QuoridorGame/
├── include/            # Header files
│   ├── AI/             # AI module (Minimax + AIPlayer)
│   ├── Core/           # Game logic (Board, Player, GameEngine, Types)
│   └── UI/             # UI layer (GameWindow, GameBoardWidget, Input)
├── src/                # Source files (mirrors include/ structure)
├── tests/              # Unit tests (test_core.cpp)
├── CMakeLists.txt      # CMake build configuration
└── build.ps1           # Windows build + deploy script
```

## Testing

```powershell
# Build and run tests
cmake -G "MinGW Makefiles" -S . -B build_test
mingw32-make -C build_test QuoridorGame_tests
.\build_test\QuoridorGame_tests.exe
```

## Technical Highlights

- **Minimax with alpha-beta pruning** — efficient game tree search
- **Asynchronous AI** — uses `QtConcurrent::run` so the UI stays responsive during AI computation
- **Custom Input translation** — converts pixel coordinates to game grid coordinates
- **BFS-based pathfinding** — `getShortestPath()` for AI evaluation and `hasPath_to_Goal()` for wall validity checks
- **Save/Load** — full game state serialization to text files

## Notes

- The AI plays as Player 2 by default (starts at the top, moves downward)
- Wall collisions and path-blocking are validated before any placement
- The minimax search depth for Hard mode (4 plies) may take several seconds on older hardware

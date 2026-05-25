#include "UI/GameWindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QApplication>
#include <QKeySequence>
#include <QTimer>
#include <QPushButton>
#include <QMetaType>
#include <QtConcurrent/QtConcurrentRun>

Q_DECLARE_METATYPE(Move)

static const char* SAVE_FILE = "savegame.txt";

// ---- construction ----

GameWindow::GameWindow(QWidget* parent)
    : QMainWindow(parent) {
    setupMenuBar();
    setupUI();
    connectSignals();
    refreshHUD();
    setWindowTitle("Quoridor");
    resize(900, 720);
}

// ---- menu bar ----

void GameWindow::setupMenuBar() {
    QMenuBar* mb = menuBar();

    QMenu* gameMenu = mb->addMenu("&Game");
    gameMenu->addAction("&New Game",  QKeySequence::New,  this, &GameWindow::resetGame);
    gameMenu->addSeparator();
    gameMenu->addAction("&Save Game", QKeySequence::Save, this, &GameWindow::saveGame);
    gameMenu->addAction("&Load Game", QKeySequence::Open, this, &GameWindow::loadGame);
    gameMenu->addSeparator();
    gameMenu->addAction("E&xit",      QKeySequence::Quit, this, &QWidget::close);
}

// ---- central UI ----

void GameWindow::setupUI() {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    // ---- top bar ----
    auto* topBar = new QWidget;
    auto* topLay = new QHBoxLayout(topBar);
    topLay->setContentsMargins(0, 0, 0, 0);

    topLay->addWidget(new QLabel("Mode:"));
    gameModeCombo = new QComboBox;
    gameModeCombo->addItems({"Human vs Human", "Human vs AI"});
    gameModeCombo->setToolTip("Switch between two-player and AI game modes");
    topLay->addWidget(gameModeCombo);

    topLay->addSpacing(12);
    topLay->addWidget(new QLabel("Difficulty:"));
    difficultyCombo = new QComboBox;
    difficultyCombo->addItems({"Easy", "Medium", "Hard"});
    difficultyCombo->setToolTip("AI search depth: Easy=1, Medium=2, Hard=4");
    topLay->addWidget(difficultyCombo);

    topLay->addSpacing(20);
    turnLabel = new QLabel("Turn: Player 1");
    turnLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
    topLay->addWidget(turnLabel);

    topLay->addSpacing(8);
    p1WallsLabel = new QLabel("P1 Walls: 10");
    p1WallsLabel->setStyleSheet("color: #4169E1; font-weight: bold;");
    topLay->addWidget(p1WallsLabel);

    p2WallsLabel = new QLabel("P2 Walls: 10");
    p2WallsLabel->setStyleSheet("color: #DC143C; font-weight: bold;");
    topLay->addWidget(p2WallsLabel);

    topLay->addStretch();

    // Save / Load toolbar buttons
    auto* saveBtn = new QPushButton("Save");
    saveBtn->setToolTip("Save game to savegame.txt");
    connect(saveBtn, &QPushButton::clicked, this, &GameWindow::saveGame);
    topLay->addWidget(saveBtn);

    auto* loadBtn = new QPushButton("Load");
    loadBtn->setToolTip("Load game from savegame.txt");
    connect(loadBtn, &QPushButton::clicked, this, &GameWindow::loadGame);
    topLay->addWidget(loadBtn);

    notificationLabel = new QLabel;
    notificationLabel->setStyleSheet("font-weight: bold;");
    topLay->addWidget(notificationLabel);

    mainLayout->addWidget(topBar);

    // ---- board ----
    boardWidget = new GameBoardWidget(&engine, this);
    mainLayout->addWidget(boardWidget, 1);
}

// ---- signal wiring ----

void GameWindow::connectSignals() {
    connect(boardWidget, &GameBoardWidget::pawnMoveClicked,
            this,       &GameWindow::onPawnMoveClicked);
    connect(boardWidget, &GameBoardWidget::wallPlaceClicked,
            this,       &GameWindow::onWallPlaceClicked);
    connect(boardWidget, &GameBoardWidget::invalidAction,
            this,       &GameWindow::onInvalidAction);

    connect(gameModeCombo,   QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,           &GameWindow::onGameModeChanged);
    connect(difficultyCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,           &GameWindow::onDifficultyChanged);

    connect(&aiWatcher, &QFutureWatcher<Move>::finished,
            this,       &GameWindow::onAIFinished);
}

// ---- slots (user actions) ----

void GameWindow::onPawnMoveClicked(Position pos) {
    if (engine.isGameOver()) return;
    if (aiRunning) return;

    if (engine.movePawn(pos)) {
        setNotification("", false);
        boardWidget->update();
        afterMove();
    } else {
        onInvalidAction(QString::fromStdString(engine.getLastError()));
    }
}

void GameWindow::onWallPlaceClicked(Wall wall) {
    if (engine.isGameOver()) return;
    if (aiRunning) return;

    if (engine.placeWall(wall)) {
        setNotification("", false);
        boardWidget->update();
        afterMove();
    } else {
        onInvalidAction(QString::fromStdString(engine.getLastError()));
    }
}

void GameWindow::onInvalidAction(const QString& msg) {
    setNotification(msg);
}

// ---- mode / difficulty changes ----

void GameWindow::onGameModeChanged(int index) {
    isAIMode = (index == 1);
    difficultyCombo->setEnabled(isAIMode);
    resetGame();
}

void GameWindow::onDifficultyChanged(int index) {
    switch (index) {
        case 0:  currentDifficulty = AIDifficulty::EASY;   break;
        case 1:  currentDifficulty = AIDifficulty::MEDIUM; break;
        default: currentDifficulty = AIDifficulty::HARD;   break;
    }
    resetGame();
}

// ---- game management ----

void GameWindow::resetGame() {
    if (aiRunning) return;  // Don't reset while AI is computing
    aiRunning = false;
    engine = GameEngine();

    if (isAIMode) {
        aiPlayer = std::make_unique<AIPlayer>(aiPlayerId, currentDifficulty);
    } else {
        aiPlayer.reset();
    }

    boardWidget->refreshValidMoves();
    boardWidget->update();
    refreshHUD();
    setNotification("New game started.", false);
}

void GameWindow::saveGame() {
    if (engine.saveGame(SAVE_FILE)) {
        setNotification("Game saved.", false);
    } else {
        setNotification("Failed to save game.");
    }
}

void GameWindow::loadGame() {
    if (engine.loadGame(SAVE_FILE)) {
        boardWidget->refreshValidMoves();
        boardWidget->update();
        refreshHUD();
        setNotification("Game loaded.", false);
        afterMove();
    } else {
        setNotification("Failed to load game.");
    }
}

// ---- post-move logic ----

void GameWindow::afterMove() {
    refreshHUD();
    boardWidget->refreshValidMoves();
    clearNotificationTimed();

    if (engine.isGameOver()) {
        showWinnerPopup(engine.getWinner());
        return;
    }

    // Trigger AI turn if applicable
    if (isAIMode && !aiRunning && engine.getCurrentTurn() == aiPlayerId) {
        aiRunning = true;
        // Use single-shot to let the UI paint before AI computation
        QTimer::singleShot(50, this, &GameWindow::triggerAITurn);
    }
}

void GameWindow::triggerAITurn() {
    setEnabled(false);
    setNotification("AI is thinking...", false);

    // Copy state for worker thread (Minimax works on copies internally)
    Board boardCopy = engine.getBoard();
    Player aiCopy = (aiPlayerId == PlayerId::PLAYER_1) ? engine.getPlayer1() : engine.getPlayer2();
    Player humanCopy = (aiPlayerId == PlayerId::PLAYER_2) ? engine.getPlayer2() : engine.getPlayer1();
    int depth = static_cast<int>(currentDifficulty);
    PlayerId id = aiPlayerId;

    aiWatcher.setFuture(QtConcurrent::run(
        [boardCopy, aiCopy, humanCopy, depth, id]() -> Move {
            Minimax brain(depth, id);
            return brain.calculateBestMove(boardCopy, aiCopy, humanCopy);
        }
    ));
}

void GameWindow::onAIFinished() {
    Move result = aiWatcher.result();
    PlayerId turnBefore = engine.getCurrentTurn();

    // Apply the AI's minimax move
    bool success = false;
    if (result.isWallPlacement) {
        success = engine.placeWall(result.wallToPlace);
    } else {
        success = engine.movePawn(result.pawnMove);
    }

    // Fallback if minimax returned an invalid move (extremely rare)
    if (!success) {
        const Player& ai = (aiPlayerId == PlayerId::PLAYER_1) ? engine.getPlayer1() : engine.getPlayer2();
        const Player& human = (aiPlayerId == PlayerId::PLAYER_2) ? engine.getPlayer2() : engine.getPlayer1();
        // Try walls near the midpoint between players (defensive, not from (0,0))
        if (ai.getWallsRemaining() > 0) {
            Board tempB = engine.getBoard();
            int cx = (ai.getPosition().x + human.getPosition().x) / 2;
            int cy = (ai.getPosition().y + human.getPosition().y) / 2;
            int x0 = (cx - 3 < 0) ? 0 : cx - 3;
            int x1 = (cx + 3 > 7) ? 7 : cx + 3;
            int y0 = (cy - 3 < 0) ? 0 : cy - 3;
            int y1 = (cy + 3 > 7) ? 7 : cy + 3;
            for (int x = x0; x <= x1 && !success; ++x) {
                for (int y = y0; y <= y1 && !success; ++y) {
                    Wall fw; fw.orientation = WallOrientation::HORIZONTAL;
                    fw.topLeft = {x, y};
                    if (tempB.isValidWallPlacement(fw) && !tempB.doesWallBlockPath(fw, engine.getPlayer1(), engine.getPlayer2()))
                        success = engine.placeWall(fw);
                    if (!success) {
                        fw.orientation = WallOrientation::VERTICAL;
                        if (tempB.isValidWallPlacement(fw) && !tempB.doesWallBlockPath(fw, engine.getPlayer1(), engine.getPlayer2()))
                            success = engine.placeWall(fw);
                    }
                }
            }
        }
        // Try forward / sideways pawn moves
        if (!success) {
            Position p = ai.getPosition();
            int dy = (ai.getGoalRow() == 0) ? -1 : 1;
            Position fwd{p.x, p.y + dy};
            if (engine.getBoard().isValidPawnMove(p, fwd, human.getPosition()))
                success = engine.movePawn(fwd);
            for (int dx : {-1, 1}) {
                if (success) break;
                Position side{p.x + dx, p.y};
                if (engine.getBoard().isValidPawnMove(p, side, human.getPosition()))
                    success = engine.movePawn(side);
            }
        }
    }

    aiRunning = false;
    setEnabled(true);
    boardWidget->update();

    if (!success) {
        setNotification("AI error — please start a new game.", true);
        return;  // Game stuck but not looping — user can click New Game
    }

    clearNotificationTimed();
    afterMove();
}

void GameWindow::clearNotificationTimed() {
    notificationLabel->setText("");
}

// ---- HUD ----

void GameWindow::refreshHUD() {
    const Player& p1 = engine.getPlayer1();
    const Player& p2 = engine.getPlayer2();

    turnLabel->setText("Turn: " + playerName(engine.getCurrentTurn()));
    p1WallsLabel->setText(QString("P1 Walls: %1").arg(p1.getWallsRemaining()));
    p2WallsLabel->setText(QString("P2 Walls: %1").arg(p2.getWallsRemaining()));
}

void GameWindow::showWinnerPopup(PlayerId winner) {
    QString msg = QString("%1 wins!").arg(playerName(winner));
    QMessageBox::information(this, "Game Over", msg);
    setNotification(msg, false);
}

void GameWindow::setNotification(const QString& msg, bool isError) {
    notificationLabel->setText(msg);
    notificationLabel->setStyleSheet(
        isError ? "color: red; font-weight: bold;"
                : "color: green; font-weight: bold;");
    if (!msg.isEmpty()) {
        QTimer::singleShot(3000, this, [this]() {
            if (notificationLabel->text() == "AI is thinking...") return;
            notificationLabel->setText("");
        });
    }
}

QString GameWindow::playerName(PlayerId id) const {
    return (id == PlayerId::PLAYER_1) ? QString("Player 1") : QString("Player 2");
}

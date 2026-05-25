#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QComboBox>
#include <QFutureWatcher>
#include <memory>
#include "../Core/GameEngine.h"
#include "../Core/Types.h"
#include "../Core/Player.h"
#include "../Core/Board.h"
#include "../AI/AIPlayer.h"
#include "../AI/Minmax.h"
#include "GameBoardWidget.h"

class GameWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit GameWindow(QWidget* parent = nullptr);
    ~GameWindow() override = default;

private slots:
    void onPawnMoveClicked(Position pos);
    void onWallPlaceClicked(Wall wall);
    void onInvalidAction(const QString& msg);
    void onGameModeChanged(int index);
    void onDifficultyChanged(int index);
    void resetGame();
    void saveGame();
    void loadGame();

private:
    // --- engine & AI ---
    GameEngine engine;
    std::unique_ptr<AIPlayer> aiPlayer;
    PlayerId aiPlayerId = PlayerId::PLAYER_2;
    AIDifficulty currentDifficulty = AIDifficulty::EASY;
    bool isAIMode = false;
    bool aiRunning = false;

    // --- widgets ---
    GameBoardWidget* boardWidget = nullptr;
    QLabel* turnLabel       = nullptr;
    QLabel* p1WallsLabel    = nullptr;
    QLabel* p2WallsLabel    = nullptr;
    QLabel* notificationLabel = nullptr;
    QComboBox* gameModeCombo   = nullptr;
    QComboBox* difficultyCombo = nullptr;

    // --- async AI ---
    QFutureWatcher<Move> aiWatcher;

    // --- helpers ---
    void setupMenuBar();
    void setupUI();
    void connectSignals();
    void afterMove();
    void triggerAITurn();
    void onAIFinished();
    void refreshHUD();
    void showWinnerPopup(PlayerId winner);
    void setNotification(const QString& msg, bool isError = true);
    void clearNotificationTimed();
    QString playerName(PlayerId id) const;
};

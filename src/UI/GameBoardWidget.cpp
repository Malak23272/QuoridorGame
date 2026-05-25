#include "UI/GameBoardWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <algorithm>
#include <optional>

GameBoardWidget::GameBoardWidget(GameEngine* engine, QWidget* parent)
    : QWidget(parent), engine(engine) {
    setMouseTracking(true);
    setMinimumSize(400, 400);
}

// ---------- drawing ----------

static const QColor BOARD_BG  = QColor("#f5e6c8");
static const QColor CELL_FILL = QColor("#f0d9b5");
static const QColor CELL_BDR  = QColor("#8b7355");
static const QColor WALL_CLR  = QColor("#5c3a1e");
static const QColor WALL_VALID   = QColor( 60, 200, 60, 160);
static const QColor WALL_INVALID = QColor(220,  40, 40, 160);
static const QColor P1_CLR    = QColor("#4169E1");
static const QColor P2_CLR    = QColor("#DC143C");
static const QColor HIGHLIGHT = QColor( 60, 200, 60, 120);

void GameBoardWidget::paintEvent(QPaintEvent* /*event*/) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Background
    p.fillRect(rect(), BOARD_BG);

    int cellSize = inputTranslator.getCellSize();
    int gapSize  = inputTranslator.getGapSize();
    if (cellSize < 1 || gapSize < 1) return;

    drawGrid(p);
    drawHighlights(p);
    drawWalls(p, engine->getBoard().getWalls());
    drawHoverWall(p);
    drawPawns(p);
}

void GameBoardWidget::drawGrid(QPainter& p) {
    int cellSize = inputTranslator.getCellSize();
    int gapSize  = inputTranslator.getGapSize();
    int margin   = inputTranslator.getMargin();
    int step     = cellSize + gapSize;

    // Board base rectangle
    int boardPx = 9 * cellSize + 8 * gapSize;
    p.fillRect(margin, margin, boardPx, boardPx, QColor("#e8c98e"));

    for (int gy = 0; gy < 9; ++gy) {
        for (int gx = 0; gx < 9; ++gx) {
            QRectF cr = cellRect(gx, gy);
            p.fillRect(cr, CELL_FILL);
            p.setPen(QPen(CELL_BDR, 1));
            p.drawRect(cr);
        }
    }
}

void GameBoardWidget::drawHighlights(QPainter& p) {
    p.setPen(Qt::NoPen);
    p.setBrush(HIGHLIGHT);
    for (const auto& pos : validHighlights) {
        p.drawRoundedRect(cellRect(pos.x, pos.y), 3, 3);
    }
}

void GameBoardWidget::drawWalls(QPainter& p, const std::vector<Wall>& walls) {
    if (walls.empty()) return;

    int cellSize = inputTranslator.getCellSize();
    int gapSize  = inputTranslator.getGapSize();
    int margin   = inputTranslator.getMargin();
    int step     = cellSize + gapSize;

    p.setPen(QPen(WALL_CLR.darker(120), 1));
    p.setBrush(WALL_CLR);

    for (const auto& w : walls) {
        if (w.orientation == WallOrientation::HORIZONTAL) {
            float x = margin + w.topLeft.x * step;
            float y = margin + (w.topLeft.y + 1) * step - gapSize;
            float wd = 2 * cellSize + gapSize;
            p.drawRect(QRectF(x, y, wd, gapSize));
        } else {
            float x = margin + (w.topLeft.x + 1) * step - gapSize;
            float y = margin + w.topLeft.y * step;
            float ht = 2 * cellSize + gapSize;
            p.drawRect(QRectF(x, y, gapSize, ht));
        }
    }
}

void GameBoardWidget::drawPawns(QPainter& p) {
    int cellSize = inputTranslator.getCellSize();
    int radius   = static_cast<int>(cellSize * 0.38);

    // Player 1
    QPointF c1 = cellCenter(engine->getPlayer1().getPosition().x,
                            engine->getPlayer1().getPosition().y);
    p.setPen(QPen(Qt::black, 2));
    p.setBrush(P1_CLR);
    p.drawEllipse(c1, radius, radius);

    // Player 1 label
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setPixelSize(radius);
    f.setBold(true);
    p.setFont(f);
    p.drawText(QRectF(c1.x() - radius, c1.y() - radius, radius * 2, radius * 2),
               Qt::AlignCenter, QString("1"));

    // Player 2
    QPointF c2 = cellCenter(engine->getPlayer2().getPosition().x,
                            engine->getPlayer2().getPosition().y);
    p.setPen(QPen(Qt::black, 2));
    p.setBrush(P2_CLR);
    p.drawEllipse(c2, radius, radius);

    p.setPen(Qt::white);
    p.drawText(QRectF(c2.x() - radius, c2.y() - radius, radius * 2, radius * 2),
               Qt::AlignCenter, QString("2"));
}

// ---------- coordinate helpers ----------

QRectF GameBoardWidget::cellRect(int gx, int gy) const {
    int step = inputTranslator.getCellSize() + inputTranslator.getGapSize();
    int m    = inputTranslator.getMargin();
    return QRectF(m + gx * step, m + gy * step,
                  inputTranslator.getCellSize(), inputTranslator.getCellSize());
}

QPointF GameBoardWidget::cellCenter(int gx, int gy) const {
    QRectF r = cellRect(gx, gy);
    return r.center();
}

// ---------- wall hover preview ----------

void GameBoardWidget::drawHoverWall(QPainter& p) {
    if (!hoverWall.has_value()) return;
    const HoverWall& hw = hoverWall.value();
    int cellSize = inputTranslator.getCellSize();
    int gapSize  = inputTranslator.getGapSize();
    int margin   = inputTranslator.getMargin();
    int step     = cellSize + gapSize;

    p.setPen(QPen(Qt::black, 2));
    p.setBrush(hw.isValid ? WALL_VALID : WALL_INVALID);

    if (hw.wall.orientation == WallOrientation::HORIZONTAL) {
        float x = margin + hw.wall.topLeft.x * step;
        float y = margin + (hw.wall.topLeft.y + 1) * step - gapSize;
        float wd = 2 * cellSize + gapSize;
        p.drawRect(QRectF(x, y, wd, gapSize));
    } else {
        float x = margin + (hw.wall.topLeft.x + 1) * step - gapSize;
        float y = margin + hw.wall.topLeft.y * step;
        float ht = 2 * cellSize + gapSize;
        p.drawRect(QRectF(x, y, gapSize, ht));
    }
}

bool GameBoardWidget::checkWallValidity(const Wall& w) const {
    if (engine->isGameOver()) return false;
    if (engine->getCurrentTurn() == PlayerId::PLAYER_1) {
        if (engine->getPlayer1().getWallsRemaining() == 0) return false;
    } else {
        if (engine->getPlayer2().getWallsRemaining() == 0) return false;
    }
    // doesWallBlockPath is non-const, so work on a board copy
    Board tempBoard = engine->getBoard();
    return tempBoard.isValidWallPlacement(w)
        && !tempBoard.doesWallBlockPath(w, engine->getPlayer1(), engine->getPlayer2());
}

// ---------- mouse ----------

void GameBoardWidget::mousePressEvent(QMouseEvent* event) {
    if (engine->isGameOver()) return;

    int mx = event->pos().x();
    int my = event->pos().y();

    if (inputTranslator.isPawnMoveClick(mx, my)) {
        emit pawnMoveClicked(inputTranslator.getGridPositionFromMouse(mx, my));
    } else if (inputTranslator.isWallPlacementClick(mx, my)) {
        emit wallPlaceClicked(inputTranslator.getWallFromMouse(mx, my));
    } else {
        emit invalidAction("Click was outside the board.");
    }
}

void GameBoardWidget::mouseMoveEvent(QMouseEvent* event) {
    int mx = event->pos().x();
    int my = event->pos().y();

    if (engine->isGameOver() || inputTranslator.isPawnMoveClick(mx, my)) {
        if (hoverWall.has_value()) {
            hoverWall.reset();
            update();
        }
        return;
    }

    if (inputTranslator.isWallPlacementClick(mx, my)) {
        Wall w = inputTranslator.getWallFromMouse(mx, my);
        bool valid = checkWallValidity(w);
        if (!hoverWall.has_value() || hoverWall.value().wall.topLeft != w.topLeft
            || hoverWall.value().wall.orientation != w.orientation) {
            hoverWall = HoverWall{w, valid};
            update();
        }
    } else {
        if (hoverWall.has_value()) {
            hoverWall.reset();
            update();
        }
    }
}

// ---------- resize ----------

void GameBoardWidget::resizeEvent(QResizeEvent* /*event*/) {
    inputTranslator.resize(width(), height());
}

// ---------- valid move highlighting ----------

void GameBoardWidget::refreshValidMoves() {
    validHighlights.clear();

    const Player& current = (engine->getCurrentTurn() == PlayerId::PLAYER_1)
                                ? engine->getPlayer1()
                                : engine->getPlayer2();
    const Player& opponent = (engine->getCurrentTurn() == PlayerId::PLAYER_1)
                                 ? engine->getPlayer2()
                                 : engine->getPlayer1();

    Position pos = current.getPosition();
    Position opp = opponent.getPosition();

    // Query the engine for every possible target cell
    for (int tx = 0; tx < 9; ++tx) {
        for (int ty = 0; ty < 9; ++ty) {
            Position target{tx, ty};
            if (engine->getBoard().isValidPawnMove(pos, target, opp)) {
                validHighlights.push_back(target);
            }
        }
    }
}

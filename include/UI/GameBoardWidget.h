#pragma once
#include <QMouseEvent>
#include <QPainter>
#include <QWidget>
#include <optional>
#include <vector>

#include "../Core/GameEngine.h"
#include "../Core/Types.h"
#include "Input.h"

class GameBoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameBoardWidget(GameEngine* engine, QWidget* parent = nullptr);

    // Recompute which cells are visually adjacent to the current player.
    void refreshValidMoves();

signals:
    void pawnMoveClicked(Position pos);
    void wallPlaceClicked(Wall wall);
    void invalidAction(const QString& message);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    GameEngine* engine;
    Input inputTranslator;

    std::vector<Position> validHighlights;

    // Hover wall preview
    struct HoverWall {
        Wall wall;
        bool isValid;
    };
    std::optional<HoverWall> hoverWall;

    // Drawing helpers
    void drawGrid(QPainter& p);
    void drawWalls(QPainter& p, const std::vector<Wall>& walls);
    void drawPawns(QPainter& p);
    void drawHighlights(QPainter& p);
    void drawHoverWall(QPainter& p);

    // Wall validity check for hover
    bool checkWallValidity(const Wall& w) const;

    // Coordinate helpers (use inputTranslator's computed values)
    QRectF cellRect(int gx, int gy) const;
    QPointF cellCenter(int gx, int gy) const;
};

#include <QApplication>
#include <QMetaType>

#include "Core/Board.h"
#include "UI/GameWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    qRegisterMetaType<Move>("Move");
    GameWindow window;
    window.show();
    return app.exec();
}

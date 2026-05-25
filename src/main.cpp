#include <QApplication>
#include <QMetaType>
#include "UI/GameWindow.h"
#include "Core/Board.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    qRegisterMetaType<Move>("Move");
    GameWindow window;
    window.show();
    return app.exec();
}

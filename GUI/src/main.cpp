#include <QApplication>
#include "TicTacToeGUI.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TicTacToeGUI game;
    game.show();

    return app.exec();
}

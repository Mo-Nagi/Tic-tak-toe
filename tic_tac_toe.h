#ifndef TIC_TAC_TOE_H
#define TIC_TAC_TOE_H

#include "game_logic.h"

class TicTacToeGame {
private:
    GameLogic gameLogic;

public:
    TicTacToeGame(char player1Symbol = 'X', char player2Symbol = 'O');
    void play();
    void reset();
};

#endif // TIC_TAC_TOE_H
#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "game_board.h"

class GameLogic {
private:
    GameBoard gameBoard;
    char currentPlayerSymbol;
    char player1Symbol;
    char player2Symbol;
    bool gameOver;
    char winner;
    int movesCount;

public:
    GameLogic(char p1Symbol = 'X', char p2Symbol = 'O');
    void resetGame();
    bool makeMove(int move);
    bool makeMove(int row, int col);
    bool isGameOver() const;
    char getWinner() const;
    bool isDraw() const;
    void displayBoard() const;
    char getCurrentPlayerSymbol() const;
    bool isValidMove(int move) const;
    bool isValidMove(int row, int col) const;
    int getMovesCount() const;
    void switchPlayerSymbols();
    const GameBoard& getBoard() const;

};

#endif // GAME_LOGIC_H

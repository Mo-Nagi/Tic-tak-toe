#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include <vector>
using namespace std;

class GameBoard {
private:
    char board[3][3];
    bool played[3][3];

public:
    GameBoard();
    void reset();
    void display() const;
    bool updateBoard(int row, int col, char symbol);
    bool updateBoardFromMoveIndex(int move, char symbol);
    bool isFull() const;
    bool isValidMove(int row, int col) const;
    bool isValidMoveIndex(int move) const;
    char getCell(int row, int col) const;
    vector<int> getAvailableMoves() const;
    bool checkWin(char symbol) const;
};

#endif // GAME_BOARD_H
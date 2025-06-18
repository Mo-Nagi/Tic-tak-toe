#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <vector>

enum class GameResult {
    PLAYER1_WIN,
    PLAYER2_WIN,
    AI_WIN,
    HUMAN_WIN,
    TIE,
    ONGOING
};

class GameBoard {
public:
    GameBoard();
    void reset();
    bool makeMove(int row, int col, char player);
    char getCell(int row, int col) const;
    GameResult checkWin() const;
    bool isFull() const;
    std::vector<std::pair<int, int>> getAvailableMoves() const;
    std::vector<std::vector<char>> getBoard() const;
    void setBoard(const std::vector<std::vector<char>>& board);

private:
    std::vector<std::vector<char>> board;
    static const int BOARD_SIZE = 3;
};

#endif // GAMEBOARD_H

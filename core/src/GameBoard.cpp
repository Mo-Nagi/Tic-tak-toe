#include "GameBoard.h"

GameBoard::GameBoard() : board(BOARD_SIZE, std::vector<char>(BOARD_SIZE, ' ')) {}

void GameBoard::reset() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = ' ';
        }
    }
}

bool GameBoard::makeMove(int row, int col, char player) {
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && board[row][col] == ' ') {
        board[row][col] = player;
        return true;
    }
    return false;
}

char GameBoard::getCell(int row, int col) const {
    if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE) {
        return board[row][col];
    }
    return ' ';
}

GameResult GameBoard::checkWin() const {
    // Check rows
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return (board[i][0] == 'X') ? GameResult::PLAYER1_WIN : GameResult::PLAYER2_WIN;
        }
    }

    // Check columns
    for (int j = 0; j < BOARD_SIZE; j++) {
        if (board[0][j] != ' ' && board[0][j] == board[1][j] && board[1][j] == board[2][j]) {
            return (board[0][j] == 'X') ? GameResult::PLAYER1_WIN : GameResult::PLAYER2_WIN;
        }
    }

    // Check diagonals
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return (board[0][0] == 'X') ? GameResult::PLAYER1_WIN : GameResult::PLAYER2_WIN;
    }
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return (board[0][2] == 'X') ? GameResult::PLAYER1_WIN : GameResult::PLAYER2_WIN;
    }

    // Check for tie
    if (isFull()) {
        return GameResult::TIE;
    }

    return GameResult::ONGOING;
}

bool GameBoard::isFull() const {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

std::vector<std::pair<int, int>> GameBoard::getAvailableMoves() const {
    std::vector<std::pair<int, int>> moves;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == ' ') {
                moves.push_back({i, j});
            }
        }
    }
    return moves;
}

std::vector<std::vector<char>> GameBoard::getBoard() const {
    return board;
}

void GameBoard::setBoard(const std::vector<std::vector<char>>& newBoard) {
    if (newBoard.size() == BOARD_SIZE && newBoard[0].size() == BOARD_SIZE) {
        board = newBoard;
    }
}

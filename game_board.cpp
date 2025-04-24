#include "game_board.h"
#include <iostream>
using namespace std;

GameBoard::GameBoard() {
    reset();
}

void GameBoard::reset() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board[i][j] = ' ';
            played[i][j] = false;
        }
    }
}

void GameBoard::display() const {
    cout << "\n";
    for (int i = 0; i < 3; ++i) {
        cout << " ";
        for (int j = 0; j < 3; ++j) {
            cout << board[i][j];
            if (j < 2) cout << " | ";
        }
        cout << "\n";
        if (i < 2) cout << "-----------\n";
    }
    cout << "\n";
}

bool GameBoard::updateBoard(int row, int col, char symbol) {
    if (row < 0 || row > 2 || col < 0 || col > 2 || played[row][col]) {
        return false;
    }
    
    board[row][col] = symbol;
    played[row][col] = true;
    return true;
}

bool GameBoard::updateBoardFromMoveIndex(int move, char symbol) {
    int row = move / 3;
    int col = move % 3;
    return updateBoard(row, col, symbol);
}

bool GameBoard::isFull() const {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (!played[i][j]) {
                return false;
            }
        }
    }
    return true;
}

bool GameBoard::isValidMove(int row, int col) const {
    return (row >= 0 && row < 3 && col >= 0 && col < 3 && !played[row][col]);
}

bool GameBoard::isValidMoveIndex(int move) const {
    if (move < 0 || move > 8) return false;
    int row = move / 3;
    int col = move % 3;
    return !played[row][col];
}

char GameBoard::getCell(int row, int col) const {
    if (row < 0 || row > 2 || col < 0 || col > 2) {
        return ' ';
    }
    return board[row][col];
}

vector<int> GameBoard::getAvailableMoves() const {
    vector<int> moves;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (!played[i][j]) {
                moves.push_back(i * 3 + j);
            }
        }
    }
    return moves;
}

bool GameBoard::checkWin(char symbol) const {
	//check rows
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) {
            return true;
        }
    }
	//check columns
    for (int j = 0; j < 3; ++j) {
        if (board[0][j] == symbol && board[1][j] == symbol && board[2][j] == symbol) {
            return true;
        }
    }
	//check diagonals
    if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) {
        return true;
    }
    if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) {
        return true;
    }
    
    return false;
}
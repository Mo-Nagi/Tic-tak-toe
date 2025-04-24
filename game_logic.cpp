#include "game_logic.h"
#include <algorithm> 

GameLogic::GameLogic(char p1Symbol, char p2Symbol)
    : player1Symbol(p1Symbol), player2Symbol(p2Symbol),
      currentPlayerSymbol(p1Symbol), gameOver(false), winner(' '), movesCount(0) {
}

void GameLogic::resetGame() {
    gameBoard.reset();
    currentPlayerSymbol = player1Symbol;
    gameOver = false;
    winner = ' ';
    movesCount = 0;
}

bool GameLogic::makeMove(int move) {
    if (gameOver || !gameBoard.isValidMoveIndex(move)) {
        return false;
    }

    if (gameBoard.updateBoardFromMoveIndex(move, currentPlayerSymbol)) {
        movesCount++;

       
        if (gameBoard.checkWin(currentPlayerSymbol)) {
            gameOver = true;
            winner = currentPlayerSymbol;
            return true;
        }

        
        if (movesCount >= 9) {
            gameOver = true;
            return true;
        }

        currentPlayerSymbol = (currentPlayerSymbol == player1Symbol) ? player2Symbol : player1Symbol;
        return true;
    }

    return false;
}

bool GameLogic::makeMove(int row, int col) {
    return makeMove(row * 3 + col);
}

bool GameLogic::isGameOver() const {
    return gameOver;
}

char GameLogic::getWinner() const {
    return winner;
}

bool GameLogic::isDraw() const {
    return gameOver && winner == ' ';
}

void GameLogic::displayBoard() const {
    gameBoard.display();
}

char GameLogic::getCurrentPlayerSymbol() const {
    return currentPlayerSymbol;
}

bool GameLogic::isValidMove(int move) const {
    return gameBoard.isValidMoveIndex(move);
}

bool GameLogic::isValidMove(int row, int col) const {
    return gameBoard.isValidMove(row, col);
}

int GameLogic::getMovesCount() const {
    return movesCount;
}

void GameLogic::switchPlayerSymbols() {
    swap(player1Symbol, player2Symbol);
    currentPlayerSymbol = player1Symbol;
}
const GameBoard& GameLogic::getBoard() const {
    return gameBoard;
}

#include "tic_tac_toe.h"
#include <iostream>
using namespace std;

TicTacToeGame::TicTacToeGame(char player1Symbol, char player2Symbol) 
    : gameLogic(player1Symbol, player2Symbol) {
}

void TicTacToeGame::play() {
    int move;
    cout << "Welcome to Tic Tac Toe!\n";
    cout << "Player 1 is " << gameLogic.getCurrentPlayerSymbol() << "\n";
    cout << "Player 2 is " << (gameLogic.getCurrentPlayerSymbol() == 'X' ? 'O' : 'X') << "\n";
    cout << "Enter moves as numbers 0-8 representing board positions from top-left to bottom-right.\n";
    
    gameLogic.displayBoard();
    
    while (!gameLogic.isGameOver()) {
        cout << "Player " << gameLogic.getCurrentPlayerSymbol() << ", enter your move (0-8): ";
        
        // Input validation
        while (!(cin >> move) || move < 0 || move > 8 || !gameLogic.isValidMove(move)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid move. Please enter a number between 0-8 for an unoccupied position: ";
        }
        
        gameLogic.makeMove(move);
        gameLogic.displayBoard();
        
        if (gameLogic.isGameOver()) {
            if (gameLogic.isDraw()) {
                cout << "Game over! It's a draw!\n";
            } else {
                cout << "Game over! Player " << gameLogic.getWinner() << " wins!\n";
            }
        }
    }
}

void TicTacToeGame::reset() {
    gameLogic.resetGame();
}
#include <iostream>
#include <vector>
using namespace std;

class Game {
    char board[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
    bool played[3][3] = {{false}};
    bool player1Win = false, player2Win = false;

    string game_id;
public:
    void displayBoard() {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                cout << board[i][j];
                if (j < 2) cout << " | ";
            }
            cout << endl;
            if (i < 2) cout << "--|---|--" << endl;
        }
    }

    void updateBoard(int move, char symbol) {
        int row = move / 3, col = move % 3;
        board[row][col] = symbol;
        played[row][col] = true;
        displayBoard();
    }

    bool checkWin() {
        // Check rows, columns, and diagonals
        for (int i = 0; i < 3; ++i) {
            if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ')
                return true;
            if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ')
                return true;
        }
        if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ')
            return true;
        if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ')
            return true;
        return false;
    }

    void play() {
        int move;
        char currentPlayer = 'O';

        for (int i = 0; i < 9; ++i) {
            cout << "Player " << currentPlayer << ", choose a square (0-8): ";
            cin >> move;

            while (move < 0 || move > 8 || played[move / 3][move % 3]) {
                cout << "Invalid move. Try again: ";
                cin >> move;
            }

            updateBoard(move, currentPlayer);

            if (checkWin()) {
                cout << "Player " << currentPlayer << " wins!" << endl;
                return;
            }
            currentPlayer = (currentPlayer == 'O') ? 'X' : 'O';
        }
        cout << "It's a draw!" << endl;
    }
};

int main() {
    Game game;
    game.play();
    return 0;
}

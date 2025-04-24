#include "game_board.h"
#include "game_logic.h"
#include "tic_tac_toe.h"
#include "tic_tac_toe_ai.h"
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <memory>

// ==================== GAME BOARD TESTS ====================
class GameBoardTest : public ::testing::Test {
protected:
    GameBoard board;
    void SetUp() override { board.reset(); }
};

TEST_F(GameBoardTest, InitialBoardState) {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_EQ(board.getCell(i, j), ' ');
            EXPECT_TRUE(board.isValidMove(i, j));
        }
    }
}

TEST_F(GameBoardTest, UpdateBoard) {
    EXPECT_TRUE(board.updateBoard(1, 1, 'X'));
    EXPECT_EQ(board.getCell(1, 1), 'X');
    EXPECT_FALSE(board.isValidMove(1, 1));
}

TEST_F(GameBoardTest, UpdateBoardInvalidCell) {
    EXPECT_FALSE(board.updateBoard(-1, 0, 'X'));
    EXPECT_FALSE(board.updateBoard(0, -1, 'X'));
    EXPECT_FALSE(board.updateBoard(3, 0, 'X'));
    EXPECT_FALSE(board.updateBoard(0, 3, 'X'));
}

TEST_F(GameBoardTest, UpdateBoardOccupiedCell) {
    board.updateBoard(1, 1, 'X');
    EXPECT_FALSE(board.updateBoard(1, 1, 'O'));
    EXPECT_EQ(board.getCell(1, 1), 'X'); // Should remain unchanged
}

TEST_F(GameBoardTest, UpdateBoardFromMoveIndex) {
    EXPECT_TRUE(board.updateBoardFromMoveIndex(4, 'X')); // Center position (1,1)
    EXPECT_EQ(board.getCell(1, 1), 'X');
    EXPECT_FALSE(board.isValidMoveIndex(4));
}

TEST_F(GameBoardTest, UpdateBoardFromInvalidMoveIndex) {
    EXPECT_FALSE(board.updateBoardFromMoveIndex(-1, 'X'));
    EXPECT_FALSE(board.updateBoardFromMoveIndex(9, 'X'));
}

TEST_F(GameBoardTest, UpdateBoardFromOccupiedMoveIndex) {
    board.updateBoardFromMoveIndex(4, 'X');
    EXPECT_FALSE(board.updateBoardFromMoveIndex(4, 'O'));
    EXPECT_EQ(board.getCell(1, 1), 'X'); // Should remain unchanged
}

TEST_F(GameBoardTest, IsValidMoveIndexBoundaryCheck) {
    EXPECT_FALSE(board.isValidMoveIndex(-1));
    EXPECT_TRUE(board.isValidMoveIndex(0));
    EXPECT_TRUE(board.isValidMoveIndex(8));
    EXPECT_FALSE(board.isValidMoveIndex(9));
}

TEST_F(GameBoardTest, GetCellBoundaryCheck) {
    EXPECT_EQ(board.getCell(-1, 0), ' ');
    EXPECT_EQ(board.getCell(0, -1), ' ');
    EXPECT_EQ(board.getCell(3, 0), ' ');
    EXPECT_EQ(board.getCell(0, 3), ' ');
}

TEST_F(GameBoardTest, EmptyBoardNotFull) {
    EXPECT_FALSE(board.isFull());
}

TEST_F(GameBoardTest, PartiallyFilledBoardNotFull) {
    board.updateBoard(0, 0, 'X');
    board.updateBoard(1, 1, 'O');
    EXPECT_FALSE(board.isFull());
}

TEST_F(GameBoardTest, FullBoard) {
    char symbols[9] = {'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X'};
    for (int i = 0; i < 9; ++i) {
        board.updateBoardFromMoveIndex(i, symbols[i]);
    }
    EXPECT_TRUE(board.isFull());
}

TEST_F(GameBoardTest, AvailableMovesEmptyBoard) {
    auto moves = board.getAvailableMoves();
    EXPECT_EQ(moves.size(), 9); // All cells should be available
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    EXPECT_EQ(moves, expected);
}

TEST_F(GameBoardTest, AvailableMovesPartiallyFilledBoard) {
    board.updateBoard(0, 0, 'X'); // 0
    board.updateBoard(1, 1, 'O'); // 4
    
    auto moves = board.getAvailableMoves();
    EXPECT_EQ(moves.size(), 7); // Two cells are now occupied
    std::vector<int> expected = {1, 2, 3, 5, 6, 7, 8};
    EXPECT_EQ(moves, expected);
}

TEST_F(GameBoardTest, AvailableMovesFullBoard) {
    char symbols[9] = {'X', 'O', 'X', 'O', 'X', 'O', 'X', 'O', 'X'};
    for (int i = 0; i < 9; ++i) {
        board.updateBoardFromMoveIndex(i, symbols[i]);
    }
    
    auto moves = board.getAvailableMoves();
    EXPECT_TRUE(moves.empty());
}

TEST_F(GameBoardTest, NoWinEmptyBoard) {
    EXPECT_FALSE(board.checkWin('X'));
    EXPECT_FALSE(board.checkWin('O'));
}

TEST_F(GameBoardTest, WinFirstRow) {
    board.updateBoard(0, 0, 'X');
    board.updateBoard(0, 1, 'X');
    board.updateBoard(0, 2, 'X');
    EXPECT_TRUE(board.checkWin('X'));
    EXPECT_FALSE(board.checkWin('O'));
}

TEST_F(GameBoardTest, WinSecondRow) {
    board.updateBoard(1, 0, 'O');
    board.updateBoard(1, 1, 'O');
    board.updateBoard(1, 2, 'O');
    EXPECT_TRUE(board.checkWin('O'));
    EXPECT_FALSE(board.checkWin('X'));
}

TEST_F(GameBoardTest, WinThirdRow) {
    board.updateBoard(2, 0, 'X');
    board.updateBoard(2, 1, 'X');
    board.updateBoard(2, 2, 'X');
    EXPECT_TRUE(board.checkWin('X'));
}

TEST_F(GameBoardTest, WinFirstColumn) {
    board.updateBoard(0, 0, 'O');
    board.updateBoard(1, 0, 'O');
    board.updateBoard(2, 0, 'O');
    EXPECT_TRUE(board.checkWin('O'));
}

TEST_F(GameBoardTest, WinSecondColumn) {
    board.updateBoard(0, 1, 'X');
    board.updateBoard(1, 1, 'X');
    board.updateBoard(2, 1, 'X');
    EXPECT_TRUE(board.checkWin('X'));
}

TEST_F(GameBoardTest, WinThirdColumn) {
    board.updateBoard(0, 2, 'O');
    board.updateBoard(1, 2, 'O');
    board.updateBoard(2, 2, 'O');
    EXPECT_TRUE(board.checkWin('O'));
}

TEST_F(GameBoardTest, WinDiagonalTopLeftToBottomRight) {
    board.updateBoard(0, 0, 'X');
    board.updateBoard(1, 1, 'X');
    board.updateBoard(2, 2, 'X');
    EXPECT_TRUE(board.checkWin('X'));
}

TEST_F(GameBoardTest, WinDiagonalTopRightToBottomLeft) {
    board.updateBoard(0, 2, 'O');
    board.updateBoard(1, 1, 'O');
    board.updateBoard(2, 0, 'O');
    EXPECT_TRUE(board.checkWin('O'));
}

TEST_F(GameBoardTest, NoWinWithDifferentSymbols) {
    board.updateBoard(0, 0, 'X');
    board.updateBoard(0, 1, 'O');
    board.updateBoard(0, 2, 'X');
    EXPECT_FALSE(board.checkWin('X'));
    EXPECT_FALSE(board.checkWin('O'));
}

TEST_F(GameBoardTest, ResetBoard) {
    board.updateBoard(0, 0, 'X');
    board.updateBoard(1, 1, 'O');
    board.reset();
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            EXPECT_EQ(board.getCell(i, j), ' ');
            EXPECT_TRUE(board.isValidMove(i, j));
        }
    }
}

// ==================== GAME LOGIC TESTS ====================
class GameLogicTest : public ::testing::Test {
protected:
    GameLogic logic{'X', 'O'};
    void SetUp() override { logic.resetGame(); }
};

TEST_F(GameLogicTest, InitialState) {
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'X');
    EXPECT_FALSE(logic.isGameOver());
    EXPECT_EQ(logic.getWinner(), ' ');
    EXPECT_EQ(logic.getMovesCount(), 0);
}

TEST_F(GameLogicTest, MakeMoveAndSwitchPlayer) {
    EXPECT_TRUE(logic.makeMove(0)); // Top-left
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'O'); // Should switch to player 2
    EXPECT_EQ(logic.getMovesCount(), 1);
}

TEST_F(GameLogicTest, MakeInvalidMove) {
    logic.makeMove(0); // First player plays top-left
    EXPECT_FALSE(logic.makeMove(0)); // Try same move again
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'O'); // Should still be player 2's turn
    EXPECT_EQ(logic.getMovesCount(), 1); // Moves count shouldn't increase
}

TEST_F(GameLogicTest, MakeOutOfBoundsMove) {
    EXPECT_FALSE(logic.makeMove(-1));
    EXPECT_FALSE(logic.makeMove(9));
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'X'); // Should still be player 1's turn
    EXPECT_EQ(logic.getMovesCount(), 0); // Moves count shouldn't increase
}

TEST_F(GameLogicTest, MakeMoveTwoDimensional) {
    EXPECT_TRUE(logic.makeMove(1, 1)); // Center
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'O');
    EXPECT_EQ(logic.getMovesCount(), 1);
}

TEST_F(GameLogicTest, MakeInvalidMoveTwoDimensional) {
    logic.makeMove(1, 1); // First player plays center
    EXPECT_FALSE(logic.makeMove(1, 1)); // Try same move again
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'O'); // Should still be player 2's turn
    EXPECT_EQ(logic.getMovesCount(), 1); // Moves count shouldn't increase
}

TEST_F(GameLogicTest, MakeOutOfBoundsMoveTwoDimensional) {
    EXPECT_FALSE(logic.makeMove(-1, 0));
    EXPECT_FALSE(logic.makeMove(0, -1));
    EXPECT_FALSE(logic.makeMove(3, 0));
    EXPECT_FALSE(logic.makeMove(0, 3));
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'X'); // Should still be player 1's turn
    EXPECT_EQ(logic.getMovesCount(), 0); // Moves count shouldn't increase
}

TEST_F(GameLogicTest, Player1WinsRow) {
    logic.makeMove(0); // X in top-left
    logic.makeMove(3); // O in middle-left
    logic.makeMove(1); // X in top-middle
    logic.makeMove(4); // O in center
    logic.makeMove(2); // X in top-right
    
    EXPECT_TRUE(logic.isGameOver());
    EXPECT_EQ(logic.getWinner(), 'X');
    EXPECT_FALSE(logic.isDraw());
}

TEST_F(GameLogicTest, Player2WinsColumn) {
    logic.makeMove(0); // X in top-left
    logic.makeMove(1); // O in top-middle
    logic.makeMove(6); // X in bottom-left
    logic.makeMove(4); // O in center
    logic.makeMove(3); // X in middle-left
    logic.makeMove(7); // O in bottom-middle
    
    EXPECT_TRUE(logic.isGameOver());
    EXPECT_EQ(logic.getWinner(), 'O');
    EXPECT_FALSE(logic.isDraw());
}

TEST_F(GameLogicTest, Player1WinsDiagonal) {
    logic.makeMove(0); // X in top-left
    logic.makeMove(1); // O in top-middle
    logic.makeMove(4); // X in center
    logic.makeMove(3); // O in middle-left
    logic.makeMove(8); // X in bottom-right
    
    EXPECT_TRUE(logic.isGameOver());
    EXPECT_EQ(logic.getWinner(), 'X');
    EXPECT_FALSE(logic.isDraw());
}

TEST_F(GameLogicTest, Player2WinsAntiDiagonal) {
    logic.makeMove(0); // X in top-left
    logic.makeMove(2); // O in top-right
    logic.makeMove(3); // X in middle-left
    logic.makeMove(4); // O in center
    logic.makeMove(7); // X in bottom-middle
    logic.makeMove(6); // O in bottom-left
    
    EXPECT_TRUE(logic.isGameOver());
    EXPECT_EQ(logic.getWinner(), 'O');
    EXPECT_FALSE(logic.isDraw());
}

TEST_F(GameLogicTest, GameDrawsFull) {
    // Play a game that ends in a draw
    // X | O | X
    // O | X | O
    // O | X | O
    int moves[9] = {0, 1, 2, 4, 3, 5, 7, 6, 8};
    for (int move : moves) {
        EXPECT_TRUE(logic.makeMove(move));
    }
    
    EXPECT_TRUE(logic.isGameOver());
    EXPECT_TRUE(logic.isDraw());
    EXPECT_EQ(logic.getWinner(), ' ');
    EXPECT_EQ(logic.getMovesCount(), 9);
}

TEST_F(GameLogicTest, NoMovesAfterGameOver) {
    // Set up a winning state
    logic.makeMove(0); // X in top-left
    logic.makeMove(3); // O in middle-left
    logic.makeMove(1); // X in top-middle
    logic.makeMove(4); // O in center
    logic.makeMove(2); // X in top-right - X wins
    
    // Try to make more moves
    EXPECT_FALSE(logic.makeMove(6));
    EXPECT_EQ(logic.getMovesCount(), 5); // Should still be 5 moves
    EXPECT_TRUE(logic.isGameOver());
}

TEST_F(GameLogicTest, SwitchPlayerSymbols) {
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'X');
    logic.switchPlayerSymbols();
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'O');
    
    // Make a move and verify turn order is preserved
    EXPECT_TRUE(logic.makeMove(0));
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'X');
}

TEST_F(GameLogicTest, ResetGame) {
    // Make some moves
    logic.makeMove(0);
    logic.makeMove(4);
    
    // Reset the game
    logic.resetGame();
    
    // Verify initial state
    EXPECT_EQ(logic.getCurrentPlayerSymbol(), 'X');
    EXPECT_FALSE(logic.isGameOver());
    EXPECT_EQ(logic.getWinner(), ' ');
    EXPECT_EQ(logic.getMovesCount(), 0);
    EXPECT_TRUE(logic.isValidMove(0));
    EXPECT_TRUE(logic.isValidMove(4));
}

TEST_F(GameLogicTest, GetMovesCount) {
    EXPECT_EQ(logic.getMovesCount(), 0);
    logic.makeMove(0);
    EXPECT_EQ(logic.getMovesCount(), 1);
    logic.makeMove(4);
    EXPECT_EQ(logic.getMovesCount(), 2);
}

// ==================== TIC TAC TOE AI TESTS ====================
class AITest : public ::testing::Test {
protected:
    std::unique_ptr<TicTacToeAI> ai;
    
    void SetUp() override {
        ai = std::make_unique<TicTacToeAI>('O', 'X');
        ai->reset();
    }
    
    // Helper to set up a specific board state for testing AI decisions
    void setupBoard(const std::vector<std::pair<int, char>>& moves) {
        ai->reset();
        
        // Apply moves to the board
        auto& board = const_cast<GameBoard&>(ai->getBoard());
        for (const auto& move : moves) {
            int row = move.first / 3;
            int col = move.first % 3;
            board.updateBoard(row, col, move.second);
        }
    }
};

TEST_F(AITest, AIPreventsRowWin) {
    // Set up a board where the player is about to win with a row
    // X | X | _
    // _ | O | _
    // _ | _ | _
    setupBoard({
        {0, 'X'}, // Top-left
        {4, 'O'}, // Center
        {1, 'X'}  // Top-middle
    });
    
    // Run the AI - it should block by playing top-right
    ai->open();
    
    // Check that AI blocked the win
    const GameBoard& board = ai->getBoard();
    EXPECT_EQ(board.getCell(0, 2), 'O'); // Should be top-right
}

TEST_F(AITest, AIPreventsColumnWin) {
    // Set up a board where the player is about to win with a column
    // X | _ | _
    // X | O | _
    // _ | _ | _
    setupBoard({
        {0, 'X'}, // Top-left
        {4, 'O'}, // Center
        {3, 'X'}  // Middle-left
    });
    
    // Run the AI - it should block by playing bottom-left
    ai->open();
    
    // Check that AI blocked the win
    const GameBoard& board = ai->getBoard();
    EXPECT_EQ(board.getCell(2, 0), 'O'); // Should be bottom-left
}

TEST_F(AITest, AIPreventsDiagonalWin) {
    // Set up a board where the player is about to win with a diagonal
    // X | _ | _
    // _ | X | _
    // _ | _ | _
    setupBoard({
        {0, 'X'}, // Top-left
        {4, 'X'}, // Center
        {1, 'O'}  // Top-middle (AI's previous move)
    });
    
    // Run the AI - it should block by playing bottom-right
    ai->open();
    
    // Check that AI blocked the win
    const GameBoard& board = ai->getBoard();
    EXPECT_EQ(board.getCell(2, 2), 'O'); // Should be bottom-right
}

TEST_F(AITest, AIGoesForWinOverBlock) {
    // Set up a board where both AI and player can win next move
    // AI should prioritize winning over blocking
    // X | X | _
    // O | O | _
    // _ | _ | _
    setupBoard({
        {0, 'X'}, // Top-left
        {3, 'O'}, // Middle-left
        {1, 'X'}, // Top-middle
        {4, 'O'}  // Center
    });
    
    // Run AI move - it should choose to win by playing middle-right
    ai->open();
    
    // Check that AI went for the win instead of blocking
    const GameBoard& board = ai->getBoard();
    EXPECT_EQ(board.getCell(1, 2), 'O'); // Should be middle-right (Win)
}

TEST_F(AITest, AITakesCenter) {
    // Empty board, AI should preferably take center
    setupBoard({
        {0, 'X'} // Human played top-left
    });
    
    // Run AI move
    ai->open();
    
    // Check that AI took center
    const GameBoard& board = ai->getBoard();
    EXPECT_EQ(board.getCell(1, 1), 'O'); // Should be center
}

TEST_F(AITest, AITakesCornerIfCenterTaken) {
    // Board with center taken, AI should take a corner
    setupBoard({
        {4, 'X'} // Human played center
    });
    
    // Run AI move
    ai->open();
    
    // Check that AI took a corner
    const GameBoard& board = ai->getBoard();
    int move = -1;
    for (int i : {0, 2, 6, 8}) { // Corner positions
        int row = i / 3;
        int col = i % 3;
        if (board.getCell(row, col) == 'O') {
            move = i;
            break;
        }
    }
    // AI should have chosen one of the corners
    EXPECT_TRUE(move == 0 || move == 2 || move == 6 || move == 8);
}

TEST_F(AITest, AIBlocksFork) {
    // Setup a potential fork situation
    // X | _ | _
    // _ | O | _
    // _ | _ | X
    setupBoard({
        {0, 'X'}, // Top-left
        {4, 'O'}, // Center
        {8, 'X'}  // Bottom-right
    });
    
    // Run AI move
    ai->open();
    
    // AI should block a potential fork by taking an edge (1, 3, 5, or 7)
    const GameBoard& board = ai->getBoard();
    int move = -1;
    for (int i : {1, 3, 5, 7}) { // Edge positions
        int row = i / 3;
        int col = i % 3;
        if (board.getCell(row, col) == 'O') {
            move = i;
            break;
        }
    }
    // AI should have chosen one of the edges to block the fork
    EXPECT_TRUE(move == 1 || move == 3 || move == 5 || move == 7);
}

TEST_F(AITest, AICreatesFork) {
    // Setup a situation where AI can create a fork
    // X | _ | _
    // _ | O | _
    // _ | _ | O
    setupBoard({
        {0, 'X'}, // Top-left
        {4, 'O'}, // Center
        {8, 'O'}  // Bottom-right
    });
    
    // Run AI move
    ai->open();
    
    // AI should create a fork
    const GameBoard& board = ai->getBoard();
    // After AI's move, there should be two ways to win
    int winningPathsCount = 0;
    
    // Check if AI can win with next move in multiple ways
    for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;
        
        if (board.getCell(row, col) == ' ') {
            // Try placing AI symbol here
            GameBoard tempBoard = board;
            tempBoard.updateBoard(row, col, 'O');
            
            if (tempBoard.checkWin('O')) {
                winningPathsCount++;
            }
        }
    }
    
    // There should be at least 2 ways to win (fork)
    EXPECT_GE(winningPathsCount, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
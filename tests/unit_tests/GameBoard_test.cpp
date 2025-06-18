#include <gtest/gtest.h>
#include "GameBoard.h"
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>

class GameBoardTest : public ::testing::Test {
protected:
    GameBoard board;
    void SetUp() override { board.reset(); }
};

// === CONSTRUCTOR TESTS ===
TEST_F(GameBoardTest, ConstructorInitializesEmptyBoard) {
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            EXPECT_EQ(board.getCell(i, j), ' ');
}

TEST_F(GameBoardTest, ConstructorCreatesBoardOfCorrectSize) {
    // Test that all valid positions are accessible
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            EXPECT_NO_THROW(board.getCell(i, j));
        }
    }
}

// === RESET FUNCTIONALITY TESTS ===
TEST_F(GameBoardTest, ResetClearsAllCells) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 1, 'O');
    board.makeMove(2, 2, 'X');
    board.reset();
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            EXPECT_EQ(board.getCell(i, j), ' ');
}

TEST_F(GameBoardTest, ResetAfterFullBoard) {
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            board.makeMove(i, j, 'X');
    board.reset();
    EXPECT_FALSE(board.isFull());
}

TEST_F(GameBoardTest, ResetAfterWinCondition) {
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'X');
    board.makeMove(0, 2, 'X');
    board.reset();
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}

TEST_F(GameBoardTest, MultipleConsecutiveResets) {
    for(int i = 0; i < 5; ++i) {
        board.makeMove(i % 3, (i + 1) % 3, 'X');
        board.reset();
        EXPECT_EQ(board.getCell(0, 0), ' ');
    }
}

// === MAKE MOVE TESTS ===
TEST_F(GameBoardTest, MakeMoveValidPosition) {
    EXPECT_TRUE(board.makeMove(1, 1, 'X'));
    EXPECT_EQ(board.getCell(1, 1), 'X');
}

TEST_F(GameBoardTest, MakeMoveAllValidPositions) {
    char symbol = 'X';
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            EXPECT_TRUE(board.makeMove(i, j, symbol));
            EXPECT_EQ(board.getCell(i, j), symbol);
            board.reset();
            symbol = (symbol == 'X') ? 'O' : 'X';
        }
    }
}

TEST_F(GameBoardTest, MakeMoveInvalidRowNegative) {
    EXPECT_FALSE(board.makeMove(-1, 0, 'X'));
    EXPECT_FALSE(board.makeMove(-5, 1, 'O'));
}

TEST_F(GameBoardTest, MakeMoveInvalidRowTooLarge) {
    EXPECT_FALSE(board.makeMove(3, 0, 'X'));
    EXPECT_FALSE(board.makeMove(10, 1, 'O'));
}

TEST_F(GameBoardTest, MakeMoveInvalidColNegative) {
    EXPECT_FALSE(board.makeMove(0, -1, 'X'));
    EXPECT_FALSE(board.makeMove(1, -3, 'O'));
}

TEST_F(GameBoardTest, MakeMoveInvalidColTooLarge) {
    EXPECT_FALSE(board.makeMove(0, 3, 'X'));
    EXPECT_FALSE(board.makeMove(1, 5, 'O'));
}

TEST_F(GameBoardTest, MakeMoveOccupiedCell) {
    board.makeMove(1, 1, 'X');
    EXPECT_FALSE(board.makeMove(1, 1, 'O'));
    EXPECT_FALSE(board.makeMove(1, 1, 'X'));
    EXPECT_EQ(board.getCell(1, 1), 'X');
}

TEST_F(GameBoardTest, MakeMoveWithDifferentSymbols) {
    EXPECT_TRUE(board.makeMove(0, 0, 'A'));
    EXPECT_TRUE(board.makeMove(0, 1, '1'));
    EXPECT_TRUE(board.makeMove(0, 2, '@'));
    EXPECT_EQ(board.getCell(0, 0), 'A');
    EXPECT_EQ(board.getCell(0, 1), '1');
    EXPECT_EQ(board.getCell(0, 2), '@');
}

// === GET CELL TESTS ===
TEST_F(GameBoardTest, GetCellValidPositions) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 2, 'O');
    board.makeMove(2, 1, 'X');
    
    EXPECT_EQ(board.getCell(0, 0), 'X');
    EXPECT_EQ(board.getCell(1, 2), 'O');
    EXPECT_EQ(board.getCell(2, 1), 'X');
    EXPECT_EQ(board.getCell(1, 1), ' '); // Empty cell
}

TEST_F(GameBoardTest, GetCellInvalidPositionsReturnSpace) {
    EXPECT_EQ(board.getCell(-1, 0), ' ');
    EXPECT_EQ(board.getCell(0, -1), ' ');
    EXPECT_EQ(board.getCell(3, 0), ' ');
    EXPECT_EQ(board.getCell(0, 3), ' ');
    EXPECT_EQ(board.getCell(-10, -10), ' ');
    EXPECT_EQ(board.getCell(100, 100), ' ');
}

TEST_F(GameBoardTest, GetCellAllEmptyPositions) {
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            EXPECT_EQ(board.getCell(i, j), ' ');
}

// === WIN DETECTION TESTS - HORIZONTAL ===
TEST_F(GameBoardTest, CheckWinHorizontalRow0X) {
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'X');
    board.makeMove(0, 2, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER1_WIN);
}

TEST_F(GameBoardTest, CheckWinHorizontalRow1O) {
    board.makeMove(1, 0, 'O');
    board.makeMove(1, 1, 'O');
    board.makeMove(1, 2, 'O');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER2_WIN);
}

TEST_F(GameBoardTest, CheckWinHorizontalRow2X) {
    board.makeMove(2, 0, 'X');
    board.makeMove(2, 1, 'X');
    board.makeMove(2, 2, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER1_WIN);
}

TEST_F(GameBoardTest, CheckWinHorizontalIncomplete) {
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}

TEST_F(GameBoardTest, CheckWinHorizontalMixed) {
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'O');
    board.makeMove(0, 2, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}

// === WIN DETECTION TESTS - VERTICAL ===
TEST_F(GameBoardTest, CheckWinVerticalCol0O) {
    board.makeMove(0, 0, 'O');
    board.makeMove(1, 0, 'O');
    board.makeMove(2, 0, 'O');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER2_WIN);
}

TEST_F(GameBoardTest, CheckWinVerticalCol1X) {
    board.makeMove(0, 1, 'X');
    board.makeMove(1, 1, 'X');
    board.makeMove(2, 1, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER1_WIN);
}

TEST_F(GameBoardTest, CheckWinVerticalCol2O) {
    board.makeMove(0, 2, 'O');
    board.makeMove(1, 2, 'O');
    board.makeMove(2, 2, 'O');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER2_WIN);
}

TEST_F(GameBoardTest, CheckWinVerticalIncomplete) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 0, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}

// === WIN DETECTION TESTS - DIAGONAL ===
TEST_F(GameBoardTest, CheckWinDiagonalMainX) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 1, 'X');
    board.makeMove(2, 2, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER1_WIN);
}

TEST_F(GameBoardTest, CheckWinDiagonalAntiO) {
    board.makeMove(0, 2, 'O');
    board.makeMove(1, 1, 'O');
    board.makeMove(2, 0, 'O');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER2_WIN);
}

TEST_F(GameBoardTest, CheckWinDiagonalMainIncomplete) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 1, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}

TEST_F(GameBoardTest, CheckWinDiagonalAntiIncomplete) {
    board.makeMove(0, 2, 'O');
    board.makeMove(1, 1, 'O');
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}

// === TIE DETECTION TESTS ===
TEST_F(GameBoardTest, CheckWinTiePattern1) {
    std::vector<std::vector<char>> tieBoard = {
        {'X','O','X'},
        {'X','O','O'},
        {'O','X','X'}
    };
    board.setBoard(tieBoard);
    EXPECT_EQ(board.checkWin(), GameResult::TIE);
}

TEST_F(GameBoardTest, CheckWinTiePattern2) {
    std::vector<std::vector<char>> tieBoard = {
        {'O','X','O'},
        {'X','X','O'},
        {'X','O','X'}
    };
    board.setBoard(tieBoard);
    EXPECT_EQ(board.checkWin(), GameResult::TIE);
}
TEST_F(GameBoardTest, CheckWinTiePattern3) {
    std::vector<std::vector<char>> tieBoard = {
        {'X','O','X'},
        {'O','X','O'},
        {'O','X','O'}
    };
    board.setBoard(tieBoard);
    EXPECT_EQ(board.checkWin(), GameResult::TIE);
}

// === ONGOING GAME TESTS ===
TEST_F(GameBoardTest, CheckWinOngoingEmpty) {
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}

TEST_F(GameBoardTest, CheckWinOngoingSingleMove) {
    board.makeMove(1, 1, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}

TEST_F(GameBoardTest, CheckWinOngoingPartialGame) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 1, 'O');
    board.makeMove(2, 2, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}

// === IS FULL TESTS ===
TEST_F(GameBoardTest, IsFullEmptyBoard) {
    EXPECT_FALSE(board.isFull());
}

TEST_F(GameBoardTest, IsFullPartialBoard) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 1, 'O');
    EXPECT_FALSE(board.isFull());
}

TEST_F(GameBoardTest, IsFullCompleteBoard) {
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            board.makeMove(i, j, 'X');
    EXPECT_TRUE(board.isFull());
}

TEST_F(GameBoardTest, IsFullAlmostFull) {
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            if(!(i == 2 && j == 2)) {
                board.makeMove(i, j, 'X');
            }
        }
    }
    EXPECT_FALSE(board.isFull());
    board.makeMove(2, 2, 'O');
    EXPECT_TRUE(board.isFull());
}

// === GET AVAILABLE MOVES TESTS ===
TEST_F(GameBoardTest, GetAvailableMovesEmptyBoard) {
    auto moves = board.getAvailableMoves();
    EXPECT_EQ(moves.size(), 9);
}

TEST_F(GameBoardTest, GetAvailableMovesPartialBoard) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 1, 'O');
    board.makeMove(2, 2, 'X');
    auto moves = board.getAvailableMoves();
    EXPECT_EQ(moves.size(), 6);
}

TEST_F(GameBoardTest, GetAvailableMovesFullBoard) {
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            board.makeMove(i, j, 'X');
    auto moves = board.getAvailableMoves();
    EXPECT_EQ(moves.size(), 0);
}

TEST_F(GameBoardTest, GetAvailableMovesCorrectPositions) {
    board.makeMove(0, 0, 'X');
    board.makeMove(2, 2, 'O');
    auto moves = board.getAvailableMoves();
    
    bool found00 = false, found22 = false;
    for(const auto& move : moves) {
        if(move.first == 0 && move.second == 0) found00 = true;
        if(move.first == 2 && move.second == 2) found22 = true;
    }
    EXPECT_FALSE(found00);
    EXPECT_FALSE(found22);
    EXPECT_EQ(moves.size(), 7);
}

TEST_F(GameBoardTest, GetAvailableMovesAfterReset) {
    board.makeMove(1, 1, 'X');
    board.reset();
    auto moves = board.getAvailableMoves();
    EXPECT_EQ(moves.size(), 9);
}

// === GET/SET BOARD TESTS ===
TEST_F(GameBoardTest, GetBoardReflectsCurrentState) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 1, 'O');
    auto boardState = board.getBoard();
    EXPECT_EQ(boardState[0][0], 'X');
    EXPECT_EQ(boardState[1][1], 'O');
    EXPECT_EQ(boardState[2][2], ' ');
}

TEST_F(GameBoardTest, SetBoardValidSize) {
    std::vector<std::vector<char>> newBoard = {
        {'X','O','X'},
        {'O','X','O'},
        {'X','O','X'}
    };
    board.setBoard(newBoard);
    for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
            EXPECT_EQ(board.getCell(i, j), newBoard[i][j]);
}

TEST_F(GameBoardTest, SetBoardInvalidSize) {
    std::vector<std::vector<char>> invalidBoard = {
        {'X','O'},
        {'O','X'}
    };
    board.makeMove(1, 1, 'X'); // Put something on board first
    board.setBoard(invalidBoard);
    // Board should remain unchanged
    EXPECT_EQ(board.getCell(1, 1), 'X');
}

TEST_F(GameBoardTest, SetBoardEmptyVector) {
    std::vector<std::vector<char>> emptyBoard;
    board.makeMove(0, 0, 'X');
    board.setBoard(emptyBoard);
    // Board should remain unchanged
    EXPECT_EQ(board.getCell(0, 0), 'X');
}

// === EDGE CASE TESTS ===
TEST_F(GameBoardTest, WinOnLastMove) {
    std::vector<std::vector<char>> almostWin = {
        {'X','O','X'},
        {'O','X','O'},
        {'O','X',' '}
    };
    board.setBoard(almostWin);
    board.makeMove(2, 2, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER1_WIN);
}

TEST_F(GameBoardTest, MultipleWinConditions) {
    // Both horizontal and vertical win for X
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'X');
    board.makeMove(0, 2, 'X');
    board.makeMove(1, 0, 'X');
    board.makeMove(2, 0, 'X');
    EXPECT_EQ(board.checkWin(), GameResult::PLAYER1_WIN);
}

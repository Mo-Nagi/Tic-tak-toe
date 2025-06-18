#include <gtest/gtest.h>
#include "GameStateStack.h"
#include <vector>
#include <chrono>
#include <iostream>

class GameStateStackTest : public ::testing::Test {
protected:
    GameStateStack stack;
    Move validMove{1, 1, 'X'};
    std::vector<std::vector<char>> validBoard;
    
    void SetUp() override {
        validBoard = std::vector<std::vector<char>>(3, std::vector<char>(3, ' '));
        validBoard[1][1] = 'X';
    }
};

// === CONSTRUCTOR/DESTRUCTOR TESTS ===
TEST_F(GameStateStackTest, ConstructorInitializesEmpty) {
    GameStateStack newStack;
    EXPECT_EQ(newStack.size(), 0);
    EXPECT_FALSE(newStack.canUndo());
}

TEST_F(GameStateStackTest, DestructorClearsStack) {
    {
        GameStateStack tempStack;
        tempStack.pushMove(validMove, validBoard);
        EXPECT_EQ(tempStack.size(), 1);
    } // Destructor called here
    // Stack should be cleaned up without issues
}

// === PUSH MOVE TESTS ===
TEST_F(GameStateStackTest, PushMoveIncrementsSize) {
    EXPECT_EQ(stack.size(), 0);
    stack.pushMove(validMove, validBoard);
    EXPECT_EQ(stack.size(), 1);
}

TEST_F(GameStateStackTest, PushMultipleMovesIncrementsSize) {
    for(int i = 0; i < 5; ++i) {
        stack.pushMove({i, i, 'X'}, validBoard);
        EXPECT_EQ(stack.size(), i + 1);
    }
}

TEST_F(GameStateStackTest, PushMoveWithDifferentSymbols) {
    stack.pushMove({0, 0, 'X'}, validBoard);
    stack.pushMove({0, 1, 'O'}, validBoard);
    stack.pushMove({0, 2, '@'}, validBoard);
    EXPECT_EQ(stack.size(), 3);
}

TEST_F(GameStateStackTest, PushMoveWithInvalidCoordinates) {
    stack.pushMove({-1, -1, 'X'}, validBoard);
    stack.pushMove({10, 10, 'O'}, validBoard);
    EXPECT_EQ(stack.size(), 2);
}

TEST_F(GameStateStackTest, PushMoveWithEmptyBoard) {
    std::vector<std::vector<char>> emptyBoard;
    stack.pushMove(validMove, emptyBoard);
    EXPECT_EQ(stack.size(), 1);
}

TEST_F(GameStateStackTest, PushMoveWithDifferentBoardSizes) {
    std::vector<std::vector<char>> smallBoard(2, std::vector<char>(2, 'X'));
    std::vector<std::vector<char>> largeBoard(4, std::vector<char>(4, 'O'));
    
    stack.pushMove(validMove, smallBoard);
    stack.pushMove(validMove, largeBoard);
    EXPECT_EQ(stack.size(), 2);
}

// === POP MOVE TESTS ===
TEST_F(GameStateStackTest, PopMoveReturnsLastPushed) {
    Move testMove{2, 1, 'O'};
    stack.pushMove(testMove, validBoard);
    Move popped = stack.popMove();
    EXPECT_EQ(popped.row, testMove.row);
    EXPECT_EQ(popped.col, testMove.col);
    EXPECT_EQ(popped.player, testMove.player);
}

TEST_F(GameStateStackTest, PopMoveDecrementsSize) {
    stack.pushMove(validMove, validBoard);
    EXPECT_EQ(stack.size(), 1);
    stack.popMove();
    EXPECT_EQ(stack.size(), 0);
}

TEST_F(GameStateStackTest, PopMoveEmptyStackReturnsInvalid) {
    Move invalid = stack.popMove();
    EXPECT_EQ(invalid.row, -1);
    EXPECT_EQ(invalid.col, -1);
    EXPECT_EQ(invalid.player, ' ');
}

TEST_F(GameStateStackTest, PopMoveOrderIsLIFO) {
    Move move1{0, 0, 'X'};
    Move move2{1, 1, 'O'};
    Move move3{2, 2, 'X'};
    
    stack.pushMove(move1, validBoard);
    stack.pushMove(move2, validBoard);
    stack.pushMove(move3, validBoard);
    
    EXPECT_EQ(stack.popMove().row, 2);
    EXPECT_EQ(stack.popMove().row, 1);
    EXPECT_EQ(stack.popMove().row, 0);
}

TEST_F(GameStateStackTest, PopMoveAfterMultiplePushes) {
    for(int i = 0; i < 10; ++i) {
        stack.pushMove({i, i, 'X'}, validBoard);
    }
    
    for(int i = 9; i >= 0; --i) {
        Move popped = stack.popMove();
        EXPECT_EQ(popped.row, i);
        EXPECT_EQ(popped.col, i);
    }
}

// === POP BOARD STATE TESTS ===
TEST_F(GameStateStackTest, PopBoardStateReturnsLastPushed) {
    std::vector<std::vector<char>> testBoard(3, std::vector<char>(3, 'O'));
    stack.pushMove(validMove, testBoard);
    auto poppedBoard = stack.popBoardState();
    EXPECT_EQ(poppedBoard[0][0], 'O');
    EXPECT_EQ(poppedBoard[2][2], 'O');
}

TEST_F(GameStateStackTest, PopBoardStateEmptyStackReturnsDefault) {
    auto board = stack.popBoardState();
    EXPECT_EQ(board.size(), 3);
    EXPECT_EQ(board[0].size(), 3);
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            EXPECT_EQ(board[i][j], ' ');
        }
    }
}

TEST_F(GameStateStackTest, PopBoardStateOrderIsLIFO) {
    std::vector<std::vector<char>> board1(3, std::vector<char>(3, 'A'));
    std::vector<std::vector<char>> board2(3, std::vector<char>(3, 'B'));
    std::vector<std::vector<char>> board3(3, std::vector<char>(3, 'C'));
    
    stack.pushMove(validMove, board1);
    stack.pushMove(validMove, board2);
    stack.pushMove(validMove, board3);
    
    EXPECT_EQ(stack.popBoardState()[0][0], 'C');
    EXPECT_EQ(stack.popBoardState()[0][0], 'B');
    EXPECT_EQ(stack.popBoardState()[0][0], 'A');
}

// === CAN UNDO TESTS ===
TEST_F(GameStateStackTest, CanUndoEmptyStack) {
    EXPECT_FALSE(stack.canUndo());
}

TEST_F(GameStateStackTest, CanUndoAfterPush) {
    stack.pushMove(validMove, validBoard);
    EXPECT_TRUE(stack.canUndo());
}

TEST_F(GameStateStackTest, CanUndoAfterPushAndPop) {
    stack.pushMove(validMove, validBoard);
    stack.popMove();
    stack.popBoardState();
    EXPECT_FALSE(stack.canUndo());
}

TEST_F(GameStateStackTest, CanUndoAfterMultiplePushes) {
    for(int i = 0; i < 5; ++i) {
        stack.pushMove({i, i, 'X'}, validBoard);
        EXPECT_TRUE(stack.canUndo());
    }
}

// === CLEAR STACK TESTS ===
TEST_F(GameStateStackTest, ClearStackEmptiesAll) {
    for(int i = 0; i < 5; ++i) {
        stack.pushMove({i, i, 'X'}, validBoard);
    }
    EXPECT_EQ(stack.size(), 5);
    stack.clearStack();
    EXPECT_EQ(stack.size(), 0);
    EXPECT_FALSE(stack.canUndo());
}

TEST_F(GameStateStackTest, ClearStackOnEmptyStack) {
    EXPECT_NO_THROW(stack.clearStack());
    EXPECT_EQ(stack.size(), 0);
}

TEST_F(GameStateStackTest, ClearStackMultipleTimes) {
    stack.pushMove(validMove, validBoard);
    stack.clearStack();
    stack.clearStack(); // Should not cause issues
    EXPECT_EQ(stack.size(), 0);
}

// === SIZE TESTS ===
TEST_F(GameStateStackTest, SizeInitiallyZero) {
    EXPECT_EQ(stack.size(), 0);
}

TEST_F(GameStateStackTest, SizeReflectsPushPop) {
    EXPECT_EQ(stack.size(), 0);
    stack.pushMove(validMove, validBoard);
    EXPECT_EQ(stack.size(), 1);
    stack.pushMove(validMove, validBoard);
    EXPECT_EQ(stack.size(), 2);
    stack.popMove();
    stack.popBoardState();
    EXPECT_EQ(stack.size(), 1);
}

TEST_F(GameStateStackTest, SizeAfterClear) {
    for(int i = 0; i < 10; ++i) {
        stack.pushMove({i, i, 'X'}, validBoard);
    }
    stack.clearStack();
    EXPECT_EQ(stack.size(), 0);
}

// === TOP MOVE TESTS ===
TEST_F(GameStateStackTest, TopMoveReturnsLastPushed) {
    Move testMove{2, 1, 'O'};
    stack.pushMove(validMove, validBoard);
    stack.pushMove(testMove, validBoard);
    Move top = stack.topMove();
    EXPECT_EQ(top.row, testMove.row);
    EXPECT_EQ(top.col, testMove.col);
    EXPECT_EQ(top.player, testMove.player);
}

TEST_F(GameStateStackTest, TopMoveDoesNotModifyStack) {
    stack.pushMove(validMove, validBoard);
    size_t sizeBefore = stack.size();
    stack.topMove();
    EXPECT_EQ(stack.size(), sizeBefore);
}

TEST_F(GameStateStackTest, TopMoveEmptyStackReturnsInvalid) {
    Move top = stack.topMove();
    EXPECT_EQ(top.row, -1);
    EXPECT_EQ(top.col, -1);
    EXPECT_EQ(top.player, ' ');
}

// === TOP BOARD STATE TESTS ===
TEST_F(GameStateStackTest, TopBoardStateReturnsLastPushed) {
    std::vector<std::vector<char>> testBoard(3, std::vector<char>(3, 'Z'));
    stack.pushMove(validMove, validBoard);
    stack.pushMove(validMove, testBoard);
    auto top = stack.topBoardState();
    EXPECT_EQ(top[0][0], 'Z');
}

TEST_F(GameStateStackTest, TopBoardStateDoesNotModifyStack) {
    stack.pushMove(validMove, validBoard);
    size_t sizeBefore = stack.size();
    stack.topBoardState();
    EXPECT_EQ(stack.size(), sizeBefore);
}

TEST_F(GameStateStackTest, TopBoardStateEmptyStackReturnsDefault) {
    auto board = stack.topBoardState();
    EXPECT_EQ(board.size(), 3);
    EXPECT_EQ(board[0].size(), 3);
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            EXPECT_EQ(board[i][j], ' ');
        }
    }
}

// === INTEGRATION TESTS ===
TEST_F(GameStateStackTest, PushPopSequenceConsistency) {
    Move move1{0, 0, 'X'};
    Move move2{1, 1, 'O'};
    std::vector<std::vector<char>> board1(3, std::vector<char>(3, 'A'));
    std::vector<std::vector<char>> board2(3, std::vector<char>(3, 'B'));
    
    stack.pushMove(move1, board1);
    stack.pushMove(move2, board2);
    
    auto poppedBoard2 = stack.popBoardState();
    auto poppedMove2 = stack.popMove();
    auto poppedBoard1 = stack.popBoardState();
    auto poppedMove1 = stack.popMove();
    
    EXPECT_EQ(poppedMove2.row, move2.row);
    EXPECT_EQ(poppedMove1.row, move1.row);
    EXPECT_EQ(poppedBoard2[0][0], 'B');
    EXPECT_EQ(poppedBoard1[0][0], 'A');
}

TEST_F(GameStateStackTest, InterleavedOperations) {
    stack.pushMove({0, 0, 'X'}, validBoard);
    stack.pushMove({1, 1, 'O'}, validBoard);
    stack.popMove();
    stack.popBoardState();
    stack.pushMove({2, 2, 'X'}, validBoard);
    EXPECT_EQ(stack.size(), 2);
    EXPECT_EQ(stack.topMove().row, 2);
}

// === STRESS TESTS ===
TEST_F(GameStateStackTest, StressTestManyOperations) {
    for(int i = 0; i < 1000; ++i) {
        stack.pushMove({i % 3, (i + 1) % 3, 'X'}, validBoard);
    }
    EXPECT_EQ(stack.size(), 1000);
    
    for(int i = 0; i < 1000; ++i) {
        stack.popMove();
        stack.popBoardState();
    }
    EXPECT_EQ(stack.size(), 0);
}


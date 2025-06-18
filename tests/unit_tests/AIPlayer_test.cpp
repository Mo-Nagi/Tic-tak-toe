#include <gtest/gtest.h>
#include "AIPlayer.h"
#include "GameBoard.h"
#include <set>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <chrono>

class AIPlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        board.reset();
        ai.clearAIMoveHistory();
    }
    
    AIPlayer ai{'O', 'X', DifficultyLevel::HARD};
    GameBoard board;
    
    bool IsValidMove(const std::pair<int, int>& move) {
        return move.first >= 0 && move.first <= 2 && 
               move.second >= 0 && move.second <= 2 &&
               board.getCell(move.first, move.second) == ' ';
    }
    
    void CreateBoardState(const std::vector<std::vector<char>>& state) {
        board.reset();
        for(int i = 0; i < 3; ++i) {
            for(int j = 0; j < 3; ++j) {
                if(state[i][j] != ' ') {
                    board.makeMove(i, j, state[i][j]);
                }
            }
        }
    }
};

// === CONSTRUCTOR TESTS ===
TEST_F(AIPlayerTest, ConstructorSetsCorrectSymbols) {
    AIPlayer test_ai('X', 'O', DifficultyLevel::HARD);
    GameBoard test_board;
    test_board.makeMove(0, 0, 'X');
    test_board.makeMove(0, 1, 'X');
    auto move = test_ai.getBestMove(test_board);
    EXPECT_EQ(move, std::make_pair(0, 2)); // Should complete the win with 'X'
}

TEST_F(AIPlayerTest, ConstructorSetsCorrectDifficulty) {
    AIPlayer easy_ai('O', 'X', DifficultyLevel::EASY);
    std::set<std::pair<int, int>> moves;
    for(int i = 0; i < 20; ++i) {
        auto move = easy_ai.getBestMove(board);
        moves.insert(move);
        easy_ai.clearAIMoveHistory();
    }
    EXPECT_GT(moves.size(), 3); // Should have variety due to randomness
}

TEST_F(AIPlayerTest, ConstructorWithSameSymbols) {
    EXPECT_NO_THROW(AIPlayer('X', 'X', DifficultyLevel::MEDIUM));
}

TEST_F(AIPlayerTest, ConstructorWithSpecialCharacters) {
    EXPECT_NO_THROW(AIPlayer('@', '#', DifficultyLevel::HARD));
}

// === DIFFICULTY SETTING TESTS ===
TEST_F(AIPlayerTest, SetDifficultyToEasy) {
    ai.setDifficulty(DifficultyLevel::EASY);
    std::set<std::pair<int, int>> moves;
    for(int i = 0; i < 15; ++i) {
        auto move = ai.getBestMove(board);
        moves.insert(move);
        ai.clearAIMoveHistory();
    }
    EXPECT_GT(moves.size(), 2); // Should show randomness
}

TEST_F(AIPlayerTest, SetDifficultyToMedium) {
    ai.setDifficulty(DifficultyLevel::MEDIUM);
    board.makeMove(0, 0, 'O');
    board.makeMove(0, 1, 'O');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(0, 2)); // Should win
}

TEST_F(AIPlayerTest, SetDifficultyToHard) {
    ai.setDifficulty(DifficultyLevel::HARD);
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'X');
    auto move1 = ai.getBestMove(board);
    ai.clearAIMoveHistory();
    auto move2 = ai.getBestMove(board);
    EXPECT_EQ(move1, move2); // Should be consistent
    EXPECT_EQ(move1, std::make_pair(0, 2)); // Should block
}

// === MOVE HISTORY TESTS ===
TEST_F(AIPlayerTest, PushAIMoveAddsToHistory) {
    EXPECT_FALSE(ai.hasAIMoveHistory());
    ai.pushAIMove(1, 2);
    EXPECT_TRUE(ai.hasAIMoveHistory());
}

TEST_F(AIPlayerTest, PopAIMoveReturnsCorrectMove) {
    ai.pushAIMove(2, 1);
    auto move = ai.popAIMove();
    EXPECT_EQ(move.first, 2);
    EXPECT_EQ(move.second, 1);
}

TEST_F(AIPlayerTest, PopAIMoveRemovesFromHistory) {
    ai.pushAIMove(0, 0);
    EXPECT_TRUE(ai.hasAIMoveHistory());
    ai.popAIMove();
    EXPECT_FALSE(ai.hasAIMoveHistory());
}

TEST_F(AIPlayerTest, PopEmptyHistoryReturnsInvalid) {
    ai.clearAIMoveHistory();
    auto move = ai.popAIMove();
    EXPECT_EQ(move.first, -1);
    EXPECT_EQ(move.second, -1);
}

TEST_F(AIPlayerTest, HistoryStackOrderLIFO) {
    ai.pushAIMove(0, 0);
    ai.pushAIMove(1, 1);
    ai.pushAIMove(2, 2);
    
    EXPECT_EQ(ai.popAIMove(), std::make_pair(2, 2));
    EXPECT_EQ(ai.popAIMove(), std::make_pair(1, 1));
    EXPECT_EQ(ai.popAIMove(), std::make_pair(0, 0));
}

TEST_F(AIPlayerTest, ClearAIMoveHistoryEmptiesStack) {
    ai.pushAIMove(1, 1);
    ai.pushAIMove(2, 2);
    EXPECT_TRUE(ai.hasAIMoveHistory());
    ai.clearAIMoveHistory();
    EXPECT_FALSE(ai.hasAIMoveHistory());
}

TEST_F(AIPlayerTest, HasAIMoveHistoryCorrectStatus) {
    EXPECT_FALSE(ai.hasAIMoveHistory());
    ai.pushAIMove(0, 1);
    EXPECT_TRUE(ai.hasAIMoveHistory());
    ai.popAIMove();
    EXPECT_FALSE(ai.hasAIMoveHistory());
}

// === GET BEST MOVE TESTS ===
TEST_F(AIPlayerTest, GetBestMoveEmptyBoardReturnsValid) {
    auto move = ai.getBestMove(board);
    EXPECT_TRUE(IsValidMove(move));
}

TEST_F(AIPlayerTest, GetBestMoveFullBoardReturnsInvalid) {
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            board.makeMove(i, j, 'X');
        }
    }
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(-1, -1));
}

TEST_F(AIPlayerTest, GetBestMoveAutomaticallyPushesToHistory) {
    auto move = ai.getBestMove(board);
    if(move != std::make_pair(-1, -1)) {
        EXPECT_TRUE(ai.hasAIMoveHistory());
        EXPECT_EQ(ai.popAIMove(), move);
    }
}

TEST_F(AIPlayerTest, GetBestMoveInvalidMoveNoHistory) {
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            board.makeMove(i, j, 'X');
        }
    }
    ai.clearAIMoveHistory();
    ai.getBestMove(board);
    EXPECT_FALSE(ai.hasAIMoveHistory());
}

// === WINNING MOVE DETECTION TESTS ===
TEST_F(AIPlayerTest, DetectsHorizontalWinRow0) {
    ai.setDifficulty(DifficultyLevel::HARD); // Use HARD for deterministic behavior
    board.makeMove(0, 0, 'O');
    board.makeMove(0, 1, 'O');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(0, 2));
}

TEST_F(AIPlayerTest, DetectsHorizontalWinRow1) {
    ai.setDifficulty(DifficultyLevel::HARD);
    board.makeMove(1, 0, 'O');
    board.makeMove(1, 2, 'O');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(1, 1));
}

TEST_F(AIPlayerTest, DetectsHorizontalWinRow2) {
    ai.setDifficulty(DifficultyLevel::HARD); // Changed from MEDIUM to HARD
    board.makeMove(2, 1, 'O');
    board.makeMove(2, 2, 'O');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(2, 0));
}

TEST_F(AIPlayerTest, DetectsVerticalWinCol0) {
    ai.setDifficulty(DifficultyLevel::HARD);
    board.makeMove(0, 0, 'O');
    board.makeMove(1, 0, 'O');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(2, 0));
}

TEST_F(AIPlayerTest, DetectsVerticalWinCol1) {
    ai.setDifficulty(DifficultyLevel::HARD); // Changed from MEDIUM to HARD
    board.makeMove(0, 1, 'O');
    board.makeMove(2, 1, 'O');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(1, 1));
}

TEST_F(AIPlayerTest, DetectsVerticalWinCol2) {
    ai.setDifficulty(DifficultyLevel::HARD);
    board.makeMove(1, 2, 'O');
    board.makeMove(2, 2, 'O');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(0, 2));
}

TEST_F(AIPlayerTest, DetectsDiagonalWinMain) {
    ai.setDifficulty(DifficultyLevel::HARD);
    board.makeMove(0, 0, 'O');
    board.makeMove(1, 1, 'O');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(2, 2));
}

TEST_F(AIPlayerTest, DetectsDiagonalWinAnti) {
    ai.setDifficulty(DifficultyLevel::HARD); // Changed from MEDIUM to HARD
    board.makeMove(0, 2, 'O');
    board.makeMove(1, 1, 'O');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(2, 0));
}

// === BLOCKING MOVE DETECTION TESTS ===
TEST_F(AIPlayerTest, BlocksHorizontalThreatRow0) {
    ai.setDifficulty(DifficultyLevel::HARD); // Changed from MEDIUM to HARD
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'X');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(0, 2));
}

TEST_F(AIPlayerTest, BlocksVerticalThreatCol0) {
    ai.setDifficulty(DifficultyLevel::HARD);
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 0, 'X');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(2, 0));
}

TEST_F(AIPlayerTest, BlocksDiagonalThreatMain) {
    ai.setDifficulty(DifficultyLevel::HARD); // Changed from MEDIUM to HARD
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 1, 'X');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(2, 2));
}

TEST_F(AIPlayerTest, BlocksDiagonalThreatAnti) {
    ai.setDifficulty(DifficultyLevel::HARD);
    board.makeMove(0, 2, 'X');
    board.makeMove(1, 1, 'X');
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(2, 0));
}

// === PRIORITY TESTS (WIN OVER BLOCK) ===
TEST_F(AIPlayerTest, PrioritizesWinOverBlock) {
    ai.setDifficulty(DifficultyLevel::HARD); // Changed from MEDIUM to HARD
    // AI can win
    board.makeMove(0, 0, 'O');
    board.makeMove(0, 1, 'O');
    // Human can also win
    board.makeMove(1, 0, 'X');
    board.makeMove(1, 1, 'X');
    // Should prioritize winning
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(0, 2));
}

// === DIFFICULTY-SPECIFIC BEHAVIOR TESTS ===
TEST_F(AIPlayerTest, EasyModeShowsRandomness) {
    ai.setDifficulty(DifficultyLevel::EASY);
    std::set<std::pair<int, int>> moves;
    for(int i = 0; i < 25; ++i) {
        auto move = ai.getBestMove(board);
        EXPECT_TRUE(IsValidMove(move));
        moves.insert(move);
        ai.clearAIMoveHistory();
    }
    EXPECT_GE(moves.size(), 4);
}

TEST_F(AIPlayerTest, MediumModeHasRandomnessComponent) {
    ai.setDifficulty(DifficultyLevel::MEDIUM);
    // No critical moves scenario
    board.makeMove(0, 0, 'X');
    board.makeMove(2, 2, 'O');
    
    std::set<std::pair<int, int>> moves;
    for(int i = 0; i < 30; ++i) {
        auto move = ai.getBestMove(board);
        moves.insert(move);
        ai.clearAIMoveHistory();
    }
    EXPECT_GE(moves.size(), 2); // Should show variety due to 50% randomness
}

TEST_F(AIPlayerTest, HardModeIsConsistent) {
    ai.setDifficulty(DifficultyLevel::HARD);
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'X');
    
    auto move1 = ai.getBestMove(board);
    ai.clearAIMoveHistory();
    auto move2 = ai.getBestMove(board);
    
    EXPECT_EQ(move1, move2);
    EXPECT_EQ(move1, std::make_pair(0, 2));
}

// === EDGE CASE TESTS ===
TEST_F(AIPlayerTest, HandlesNearFullBoard) {
    // Fill 7 out of 9 cells
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'O');
    board.makeMove(0, 2, 'X');
    board.makeMove(1, 0, 'O');
    board.makeMove(1, 1, 'X');
    board.makeMove(1, 2, 'O');
    board.makeMove(2, 0, 'X');
    
    auto move = ai.getBestMove(board);
    EXPECT_TRUE(move == std::make_pair(2, 1) || move == std::make_pair(2, 2));
}

TEST_F(AIPlayerTest, HandlesSingleAvailableMove) {
    // Fill all except one
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            if(!(i == 2 && j == 2)) {
                board.makeMove(i, j, 'X');
            }
        }
    }
    auto move = ai.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(2, 2));
}

TEST_F(AIPlayerTest, NeverReturnsOccupiedPosition) {
    board.makeMove(0, 0, 'X');
    board.makeMove(1, 1, 'O');
    board.makeMove(2, 2, 'X');
    
    for(int i = 0; i < 20; ++i) {
        auto move = ai.getBestMove(board);
        ai.clearAIMoveHistory();
        if(move != std::make_pair(-1, -1)) {
            EXPECT_NE(move, std::make_pair(0, 0));
            EXPECT_NE(move, std::make_pair(1, 1));
            EXPECT_NE(move, std::make_pair(2, 2));
        }
    }
}

// === SYMBOL HANDLING TESTS ===
TEST_F(AIPlayerTest, WorksWithXSymbol) {
    AIPlayer ai_x('X', 'O', DifficultyLevel::HARD);
    board.makeMove(0, 0, 'X');
    board.makeMove(0, 1, 'X');
    auto move = ai_x.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(0, 2)); // Should win with X
}

TEST_F(AIPlayerTest, WorksWithOSymbol) {
    AIPlayer ai_o('O', 'X', DifficultyLevel::HARD);
    board.makeMove(0, 0, 'O');
    board.makeMove(0, 1, 'O');
    auto move = ai_o.getBestMove(board);
    EXPECT_EQ(move, std::make_pair(0, 2)); // Should win with O
}

// === PERFORMANCE TESTS ===
TEST_F(AIPlayerTest, HandlesMultipleConsecutiveCalls) {
    for(int i = 0; i < 100; ++i) {
        EXPECT_NO_THROW(ai.getBestMove(board));
        ai.clearAIMoveHistory();
    }
}

TEST_F(AIPlayerTest, HandlesComplexBoardStates) {
    std::vector<std::vector<char>> complex_board = {
        {'X', 'O', 'X'},
        {'O', 'X', ' '},
        {' ', 'X', 'O'}
    };
    CreateBoardState(complex_board);
    auto move = ai.getBestMove(board);
    EXPECT_TRUE(IsValidMove(move));
}

// === BOUNDARY TESTS ===
TEST_F(AIPlayerTest, HandlesCornerStartPositions) {
    board.makeMove(0, 0, 'X'); // Corner
    auto move = ai.getBestMove(board);
    EXPECT_TRUE(IsValidMove(move));
}

TEST_F(AIPlayerTest, HandlesCenterStartPosition) {
    board.makeMove(1, 1, 'X'); // Center
    auto move = ai.getBestMove(board);
    EXPECT_TRUE(IsValidMove(move));
}

TEST_F(AIPlayerTest, HandlesEdgeStartPositions) {
    board.makeMove(0, 1, 'X'); // Edge
    auto move = ai.getBestMove(board);
    EXPECT_TRUE(IsValidMove(move));
}

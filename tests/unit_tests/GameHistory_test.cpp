#include <gtest/gtest.h>
#include "GameHistory.h"
#include <vector>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>

class GameHistoryTest : public ::testing::Test {
protected:
    GameHistory history;
    GameRecord sampleRecord1, sampleRecord2, sampleRecord3;

    void SetUp() override {
        // Clean up any existing test files
        std::remove("game_history.dat");
        
        std::vector<std::vector<char>> board1(3, std::vector<char>(3, 'X'));
        std::vector<std::vector<char>> board2(3, std::vector<char>(3, 'O'));
        std::vector<std::vector<char>> board3(3, std::vector<char>(3, ' '));
        
        sampleRecord1 = GameRecord("alice", "bob", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER1_WIN, board1, "2025-06-11 12:00:00");
        sampleRecord2 = GameRecord("bob", "alice", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER2_WIN, board2, "2025-06-11 13:00:00");
        sampleRecord3 = GameRecord("eve", "ai", GameMode::PLAYER_VS_AI, GameResult::AI_WIN, board3, "2025-06-11 14:00:00");
    }
    
    void TearDown() override {
        std::remove("game_history.dat");
    }
};

// === CONSTRUCTOR TESTS ===
TEST_F(GameHistoryTest, ConstructorInitializesEmpty) {
    GameHistory newHistory;
    auto games = newHistory.getAllGames();
    EXPECT_TRUE(games.empty());
}

TEST_F(GameHistoryTest, ConstructorLoadsExistingHistory) {
    // Create a history file first
    history.addGameRecord(sampleRecord1);
    
    // Create new instance - should load existing data
    GameHistory newHistory;
    auto games = newHistory.getAllGames();
    EXPECT_FALSE(games.empty());
}

// === ADD GAME RECORD TESTS ===
TEST_F(GameHistoryTest, AddSingleRecord) {
    size_t before = history.getAllGames().size();
    history.addGameRecord(sampleRecord1);
    EXPECT_EQ(history.getAllGames().size(), before + 1);
}

TEST_F(GameHistoryTest, AddMultipleRecords) {
    history.addGameRecord(sampleRecord1);
    history.addGameRecord(sampleRecord2);
    history.addGameRecord(sampleRecord3);
    EXPECT_EQ(history.getAllGames().size(), 3);
}

TEST_F(GameHistoryTest, AddRecordWithEmptyPlayerNames) {
    GameRecord rec("", "", GameMode::PLAYER_VS_PLAYER, GameResult::TIE,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, ' ')), "2025-06-11 19:00:00");
    history.addGameRecord(rec);
    auto all = history.getAllGames();
    EXPECT_EQ(all.size(), 1);
    EXPECT_EQ(all[0].player1, "");
    EXPECT_EQ(all[0].player2, "");
}

TEST_F(GameHistoryTest, AddRecordWithSpecialCharacters) {
    GameRecord rec("ali@ce", "b#ob$", GameMode::PLAYER_VS_PLAYER, GameResult::TIE,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-11 23:00:00");
    history.addGameRecord(rec);
    auto all = history.getAllGames();
    EXPECT_EQ(all[0].player1, "ali@ce");
    EXPECT_EQ(all[0].player2, "b#ob$");
}

TEST_F(GameHistoryTest, AddRecordWithLongNames) {
    std::string longName(100, 'a');
    GameRecord rec(longName, "short", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER1_WIN,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-12 00:00:00");
    history.addGameRecord(rec);
    auto all = history.getAllGames();
    EXPECT_EQ(all[0].player1, longName);
}

TEST_F(GameHistoryTest, AddRecordWithSamePlayerNames) {
    GameRecord rec("same", "same", GameMode::PLAYER_VS_PLAYER, GameResult::TIE,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-12 02:00:00");
    history.addGameRecord(rec);
    auto all = history.getAllGames();
    EXPECT_EQ(all[0].player1, "same");
    EXPECT_EQ(all[0].player2, "same");
}

// === GET USER GAMES TESTS ===
TEST_F(GameHistoryTest, GetUserGamesAsPlayer1) {
    history.addGameRecord(sampleRecord1);
    auto games = history.getUserGames("alice");
    ASSERT_FALSE(games.empty());
    EXPECT_EQ(games[0].player1, "alice");
}

TEST_F(GameHistoryTest, GetUserGamesAsPlayer2) {
    history.addGameRecord(sampleRecord1);
    auto games = history.getUserGames("bob");
    ASSERT_FALSE(games.empty());
    EXPECT_EQ(games[0].player2, "bob");
}

TEST_F(GameHistoryTest, GetUserGamesUnknownUser) {
    history.addGameRecord(sampleRecord1);
    auto games = history.getUserGames("unknown");
    EXPECT_TRUE(games.empty());
}

TEST_F(GameHistoryTest, GetUserGamesEmptyHistory) {
    auto games = history.getUserGames("anyone");
    EXPECT_TRUE(games.empty());
}

TEST_F(GameHistoryTest, GetUserGamesMultipleMatches) {
    history.addGameRecord(sampleRecord1); // alice vs bob
    history.addGameRecord(sampleRecord2); // bob vs alice
    auto games = history.getUserGames("alice");
    EXPECT_EQ(games.size(), 2);
}

TEST_F(GameHistoryTest, GetUserGamesWithSpecialCharacters) {
    GameRecord rec("test@user", "other", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER1_WIN,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-11 15:00:00");
    history.addGameRecord(rec);
    auto games = history.getUserGames("test@user");
    EXPECT_EQ(games.size(), 1);
}

// === GET ALL GAMES TESTS ===
TEST_F(GameHistoryTest, GetAllGamesEmpty) {
    auto all = history.getAllGames();
    EXPECT_TRUE(all.empty());
}

TEST_F(GameHistoryTest, GetAllGamesReturnsAll) {
    history.addGameRecord(sampleRecord1);
    history.addGameRecord(sampleRecord2);
    history.addGameRecord(sampleRecord3);
    auto all = history.getAllGames();
    EXPECT_EQ(all.size(), 3);
}

TEST_F(GameHistoryTest, GetAllGamesOrderPreserved) {
    history.addGameRecord(sampleRecord1);
    history.addGameRecord(sampleRecord2);
    auto all = history.getAllGames();
    EXPECT_EQ(all[0].player1, "alice");
    EXPECT_EQ(all[1].player1, "bob");
}

// === GAME MODE TESTS ===
TEST_F(GameHistoryTest, PlayerVsPlayerMode) {
    GameRecord pvp("user1", "user2", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER1_WIN,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-11 15:00:00");
    history.addGameRecord(pvp);
    auto games = history.getUserGames("user1");
    EXPECT_EQ(games[0].mode, GameMode::PLAYER_VS_PLAYER);
}

TEST_F(GameHistoryTest, PlayerVsAIMode) {
    GameRecord pvai("user", "AI", GameMode::PLAYER_VS_AI, GameResult::AI_WIN,
                    std::vector<std::vector<char>>(3, std::vector<char>(3, 'O')), "2025-06-11 16:00:00");
    history.addGameRecord(pvai);
    auto games = history.getUserGames("user");
    EXPECT_EQ(games[0].mode, GameMode::PLAYER_VS_AI);
}

// === GAME RESULT TESTS ===
TEST_F(GameHistoryTest, Player1WinResult) {
    GameRecord win("winner", "loser", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER1_WIN,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-11 17:00:00");
    history.addGameRecord(win);
    auto games = history.getUserGames("winner");
    EXPECT_EQ(games[0].result, GameResult::PLAYER1_WIN);
}

TEST_F(GameHistoryTest, Player2WinResult) {
    GameRecord win("loser", "winner", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER2_WIN,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, 'O')), "2025-06-11 18:00:00");
    history.addGameRecord(win);
    auto games = history.getUserGames("winner");
    EXPECT_EQ(games[0].result, GameResult::PLAYER2_WIN);
}

TEST_F(GameHistoryTest, TieResult) {
    GameRecord tie("player1", "player2", GameMode::PLAYER_VS_PLAYER, GameResult::TIE,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-11 19:00:00");
    history.addGameRecord(tie);
    auto games = history.getUserGames("player1");
    EXPECT_EQ(games[0].result, GameResult::TIE);
}

TEST_F(GameHistoryTest, AIWinResult) {
    GameRecord aiWin("human", "AI", GameMode::PLAYER_VS_AI, GameResult::AI_WIN,
                     std::vector<std::vector<char>>(3, std::vector<char>(3, 'O')), "2025-06-11 20:00:00");
    history.addGameRecord(aiWin);
    auto games = history.getUserGames("human");
    EXPECT_EQ(games[0].result, GameResult::AI_WIN);
}

// === MOVES TESTS ===
TEST_F(GameHistoryTest, RecordWithNoMoves) {
    GameRecord rec = sampleRecord1;
    rec.moves.clear();
    history.addGameRecord(rec);
    auto games = history.getUserGames("alice");
    EXPECT_TRUE(games[0].moves.empty());
}

TEST_F(GameHistoryTest, RecordWithSingleMove) {
    GameRecord rec = sampleRecord1;
    rec.moves.clear();
    rec.moves.push_back(Move(1, 1, 'X'));
    history.addGameRecord(rec);
    auto games = history.getUserGames("alice");
    EXPECT_EQ(games[0].moves.size(), 1);
    EXPECT_EQ(games[0].moves[0].row, 1);
    EXPECT_EQ(games[0].moves[0].col, 1);
    EXPECT_EQ(games[0].moves[0].player, 'X');
}

TEST_F(GameHistoryTest, RecordWithMultipleMoves) {
    GameRecord rec = sampleRecord1;
    rec.moves.clear();
    rec.moves.push_back(Move(0, 0, 'X'));
    rec.moves.push_back(Move(1, 1, 'O'));
    rec.moves.push_back(Move(2, 2, 'X'));
    history.addGameRecord(rec);
    auto games = history.getUserGames("alice");
    EXPECT_EQ(games[0].moves.size(), 3);
}

TEST_F(GameHistoryTest, RecordWithMaxMoves) {
    GameRecord rec = sampleRecord1;
    rec.moves.clear();
    for(int i = 0; i < 9; ++i) {
        rec.moves.push_back(Move(i / 3, i % 3, (i % 2 == 0) ? 'X' : 'O'));
    }
    history.addGameRecord(rec);
    auto games = history.getUserGames("alice");
    EXPECT_EQ(games[0].moves.size(), 9);
}

// === BOARD STATE TESTS ===
TEST_F(GameHistoryTest, RecordWithCustomBoard) {
    std::vector<std::vector<char>> customBoard = {
        {'X', 'O', 'X'},
        {'O', 'X', 'O'},
        {'X', 'O', 'X'}
    };
    GameRecord rec("test", "user", GameMode::PLAYER_VS_PLAYER, GameResult::TIE, customBoard, "2025-06-11 21:00:00");
    history.addGameRecord(rec);
    auto games = history.getUserGames("test");
    EXPECT_EQ(games[0].finalBoard[0][0], 'X');
    EXPECT_EQ(games[0].finalBoard[1][1], 'X');
    EXPECT_EQ(games[0].finalBoard[2][2], 'X');
}

TEST_F(GameHistoryTest, RecordWithEmptyBoard) {
    std::vector<std::vector<char>> emptyBoard(3, std::vector<char>(3, ' '));
    GameRecord rec("empty", "test", GameMode::PLAYER_VS_PLAYER, GameResult::ONGOING, emptyBoard, "2025-06-11 22:00:00");
    history.addGameRecord(rec);
    auto games = history.getUserGames("empty");
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            EXPECT_EQ(games[0].finalBoard[i][j], ' ');
        }
    }
}

// === TIMESTAMP TESTS ===
TEST_F(GameHistoryTest, TimestampPreservation) {
    std::string timestamp = "2025-12-25 12:30:45";
    GameRecord rec("user1", "user2", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER1_WIN,
                   std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), timestamp);
    history.addGameRecord(rec);
    auto games = history.getUserGames("user1");
    EXPECT_EQ(games[0].timestamp, timestamp);
}


// === FILE PERSISTENCE TESTS ===
TEST_F(GameHistoryTest, SaveHistoryCreatesFile) {
    history.addGameRecord(sampleRecord1);
    std::ifstream file("game_history.dat");
    EXPECT_TRUE(file.good());
    file.close();
}

TEST_F(GameHistoryTest, LoadHistoryFromFile) {
    history.addGameRecord(sampleRecord1);
    
    GameHistory newHistory;
    auto games = newHistory.getUserGames("alice");
    EXPECT_FALSE(games.empty());
    EXPECT_EQ(games[0].player1, "alice");
}



// === PERFORMANCE TESTS ===
TEST_F(GameHistoryTest, AddManyRecords) {
    for(int i = 0; i < 100; ++i) {
        GameRecord rec("user" + std::to_string(i), "opponent", GameMode::PLAYER_VS_PLAYER, GameResult::TIE,
                       std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-11 15:00:00");
        history.addGameRecord(rec);
    }
    auto all = history.getAllGames();
    EXPECT_EQ(all.size(), 100);
}

TEST_F(GameHistoryTest, RetrieveManyUserGames) {
    for(int i = 0; i < 50; ++i) {
        GameRecord rec("alice", "opponent" + std::to_string(i), GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER1_WIN,
                       std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-11 15:00:00");
        history.addGameRecord(rec);
    }
    auto games = history.getUserGames("alice");
    EXPECT_EQ(games.size(), 50);
}

// === EDGE CASES ===
TEST_F(GameHistoryTest, DuplicateRecords) {
    history.addGameRecord(sampleRecord1);
    history.addGameRecord(sampleRecord1); // Same record again
    auto all = history.getAllGames();
    EXPECT_EQ(all.size(), 2); // Should allow duplicates
}

TEST_F(GameHistoryTest, UserInBothPlayerPositions) {
    GameRecord rec1("alice", "bob", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER1_WIN,
                    std::vector<std::vector<char>>(3, std::vector<char>(3, 'X')), "2025-06-11 12:00:00");
    GameRecord rec2("bob", "alice", GameMode::PLAYER_VS_PLAYER, GameResult::PLAYER2_WIN,
                    std::vector<std::vector<char>>(3, std::vector<char>(3, 'O')), "2025-06-11 13:00:00");
    
    history.addGameRecord(rec1);
    history.addGameRecord(rec2);
    
    auto aliceGames = history.getUserGames("alice");
    EXPECT_EQ(aliceGames.size(), 2);
}


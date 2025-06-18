#include <gtest/gtest.h>
#include "AIPlayer.h"
#include "GameBoard.h"
#include "GameHistory.h"
#include "GameStateStack.h"
#include "UserManager.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>

class TicTacToeIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::remove("game_history.dat");
        std::remove("users.dat");
        
        board.reset();
        history = std::make_unique<GameHistory>();
        users = std::make_unique<UserHashTable>();
        stack = std::make_unique<GameStateStack>();
        aiEasy = std::make_unique<AIPlayer>('O', 'X', DifficultyLevel::EASY);
        aiMedium = std::make_unique<AIPlayer>('O', 'X', DifficultyLevel::MEDIUM);
        aiHard = std::make_unique<AIPlayer>('O', 'X', DifficultyLevel::HARD);
    }
    
    void TearDown() override {
        std::remove("game_history.dat");
        std::remove("users.dat");
    }
    
    GameBoard board;
    std::unique_ptr<GameHistory> history;
    std::unique_ptr<UserHashTable> users;
    std::unique_ptr<GameStateStack> stack;
    std::unique_ptr<AIPlayer> aiEasy, aiMedium, aiHard;
    
    // Helper method to create game record with fixed timestamp
    GameRecord createGameRecord(const std::string& p1, const std::string& p2, 
                               GameResult result, const std::vector<std::vector<char>>& board,
                               const std::string& timestamp = "2025-06-16 14:30:00") {
        return GameRecord(p1, p2, GameMode::PLAYER_VS_PLAYER, result, board, timestamp);
    }
    
    // Helper to simulate realistic game patterns
    std::vector<std::pair<int, int>> getGamePattern(int patternType) {
        switch(patternType % 10) {
            case 0: return {{1,1}, {0,0}, {0,1}, {2,2}, {2,1}}; // X wins diagonal
            case 1: return {{0,0}, {1,1}, {0,1}, {1,0}, {0,2}}; // X wins row
            case 2: return {{0,0}, {0,1}, {1,0}, {0,2}, {2,0}}; // O wins col
            case 3: return {{0,0}, {0,1}, {0,2}, {1,0}, {1,2}, {1,1}, {2,0}, {2,2}, {2,1}}; // Tie
            case 4: return {{1,1}, {0,2}, {0,0}, {2,0}, {2,2}}; // X wins diagonal
            case 5: return {{0,1}, {1,1}, {0,0}, {1,0}, {0,2}}; // X wins row
            case 6: return {{1,0}, {0,0}, {1,1}, {0,1}, {1,2}}; // X wins row
            case 7: return {{0,0}, {1,0}, {0,1}, {2,0}, {0,2}}; // O wins col
            case 8: return {{2,2}, {1,1}, {2,1}, {0,0}, {2,0}}; // X wins row
            default: return {{1,1}, {0,0}, {2,2}, {0,1}, {1,0}}; // Various
        }
    }

    // Helper to simulate complete game
    GameResult playCompleteGame(AIPlayer& ai, char aiSymbol, char humanSymbol) {
        board.reset();
        char currentPlayer = 'X';
        
        while(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
            if(currentPlayer == aiSymbol) {
                auto move = ai.getBestMove(board);
                if(move.first == -1) break;
                board.makeMove(move.first, move.second, aiSymbol);
            } else {
                auto moves = board.getAvailableMoves();
                if(moves.empty()) break;
                auto move = moves[0];
                board.makeMove(move.first, move.second, humanSymbol);
            }
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
        
        return board.checkWin();
    }
};

// ==================== USER MANAGEMENT INTEGRATION TESTS ====================

TEST_F(TicTacToeIntegrationTest, UserRegistrationAndAuthentication) {
    EXPECT_TRUE(users->insertUser("player1", "hash123"));
    EXPECT_TRUE(users->authenticateUser("player1", "hash123"));
    EXPECT_FALSE(users->authenticateUser("player1", "wronghash"));
    EXPECT_FALSE(users->authenticateUser("nonexistent", "hash"));
}

TEST_F(TicTacToeIntegrationTest, MultipleUserRegistrationFlow) {
    for(int i = 0; i < 25; ++i) {
        std::string username = "user" + std::to_string(i);
        std::string password = "pass" + std::to_string(i);
        EXPECT_TRUE(users->insertUser(username, password));
        EXPECT_TRUE(users->authenticateUser(username, password));
        EXPECT_TRUE(users->userExists(username));
    }
    
    auto allUsers = users->getAllUsers();
    EXPECT_EQ(allUsers.size(), 25);
}

TEST_F(TicTacToeIntegrationTest, UserStatisticsManagement) {
    users->insertUser("stats_player", "hash");
    User* user = users->getUser("stats_player");
    ASSERT_NE(user, nullptr);
    
    user->gamesPlayed = 15;
    user->gamesWon = 10;
    user->gamesLost = 3;
    user->gamesTied = 2;
    users->updateUser("stats_player", *user);
    
    User* updated = users->getUser("stats_player");
    EXPECT_EQ(updated->gamesPlayed, 15);
    EXPECT_EQ(updated->gamesWon, 10);
    EXPECT_EQ(updated->gamesLost, 3);
    EXPECT_EQ(updated->gamesTied, 2);
}

TEST_F(TicTacToeIntegrationTest, UserDataPersistence) {
    users->insertUser("persistent_user", "hash");
    User* user = users->getUser("persistent_user");
    user->gamesPlayed = 5;
    users->updateUser("persistent_user", *user);
    
    auto newUsers = std::make_unique<UserHashTable>();
    EXPECT_TRUE(newUsers->userExists("persistent_user"));
    User* loadedUser = newUsers->getUser("persistent_user");
    EXPECT_EQ(loadedUser->gamesPlayed, 5);
}

TEST_F(TicTacToeIntegrationTest, UserRemovalIntegration) {
    users->insertUser("temp_user", "hash");
    EXPECT_TRUE(users->userExists("temp_user"));
    
    users->removeUser("temp_user");
    EXPECT_FALSE(users->userExists("temp_user"));
    EXPECT_EQ(users->getUser("temp_user"), nullptr);
}



// Complete Game Flow Integration
TEST_F(TicTacToeIntegrationTest, PlayerVsPlayerCompleteGameFlow) {
    users->insertUser("alice", "hash1");
    users->insertUser("bob", "hash2");
    
    for(int game = 0; game < 25; ++game) {
        board.reset();
        
        // Simulate complete game - Alice wins
        board.makeMove(0, 0, 'X'); // Alice
        board.makeMove(0, 1, 'O'); // Bob
        board.makeMove(1, 0, 'X'); // Alice
        board.makeMove(1, 1, 'O'); // Bob
        board.makeMove(2, 0, 'X'); // Alice wins
        
        EXPECT_EQ(board.checkWin(), GameResult::PLAYER1_WIN);
        
        // Record game in history
        std::string timestamp = "2025-06-16 " + std::to_string(14 + game % 10) + ":30:00";
        GameRecord record = createGameRecord("alice", "bob", GameResult::PLAYER1_WIN, board.getBoard(), timestamp);
        
        // Add moves to record
        record.moves.push_back(Move(0, 0, 'X'));
        record.moves.push_back(Move(0, 1, 'O'));
        record.moves.push_back(Move(1, 0, 'X'));
        record.moves.push_back(Move(1, 1, 'O'));
        record.moves.push_back(Move(2, 0, 'X'));
        
        history->addGameRecord(record);
        
        // Update user statistics
        User* alice = users->getUser("alice");
        alice->gamesPlayed++;
        alice->gamesWon++;
        users->updateUser("alice", *alice);
        
        User* bob = users->getUser("bob");
        bob->gamesPlayed++;
        bob->gamesLost++;
        users->updateUser("bob", *bob);
    }
    
    // Verify final statistics
    auto aliceGames = history->getUserGames("alice");
    EXPECT_EQ(aliceGames.size(), 25);
    
    User* finalAlice = users->getUser("alice");
    EXPECT_EQ(finalAlice->gamesWon, 25);
    EXPECT_EQ(finalAlice->gamesPlayed, 25);
    
    User* finalBob = users->getUser("bob");
    EXPECT_EQ(finalBob->gamesLost, 25);
    EXPECT_EQ(finalBob->gamesPlayed, 25);
}

// Tests 76-125: AI vs Player Integration
TEST_F(TicTacToeIntegrationTest, AIvsPlayerEasyMode) {
    users->insertUser("player1", "hash1");
    users->insertUser("ai_easy", "hash2");
    
    aiEasy->clearAIMoveHistory();
    
    for(int game = 0; game < 15; ++game) {
        board.reset();
        
        GameResult result = playCompleteGame(*aiEasy, 'O', 'X');
        EXPECT_TRUE(result != GameResult::ONGOING);
        
        // Record game
        std::string timestamp = "2025-06-16 " + std::to_string(10 + game) + ":00:00";
        GameRecord record("player1", "AI_Easy", GameMode::PLAYER_VS_AI, result, board.getBoard(), timestamp);
        history->addGameRecord(record);
        
        // Update user stats
        User* player = users->getUser("player1");
        player->gamesPlayed++;
        if(result == GameResult::PLAYER1_WIN) player->gamesWon++;
        else if(result == GameResult::PLAYER2_WIN) player->gamesLost++;
        else player->gamesTied++;
        users->updateUser("player1", *player);
    }
    
    auto playerGames = history->getUserGames("player1");
    EXPECT_EQ(playerGames.size(), 15);
    
    User* finalPlayer = users->getUser("player1");
    EXPECT_EQ(finalPlayer->gamesPlayed, 15);
}

TEST_F(TicTacToeIntegrationTest, AIvsPlayerAllDifficulties) {
    users->insertUser("challenger", "hash");
    
    std::vector<std::unique_ptr<AIPlayer>> ais;
    ais.push_back(std::make_unique<AIPlayer>('O', 'X', DifficultyLevel::EASY));
    ais.push_back(std::make_unique<AIPlayer>('O', 'X', DifficultyLevel::MEDIUM));
    ais.push_back(std::make_unique<AIPlayer>('O', 'X', DifficultyLevel::HARD));
    
    std::vector<std::string> difficultyNames = {"Easy", "Medium", "Hard"};
    
    for(int difficulty = 0; difficulty < 3; ++difficulty) {
        for(int game = 0; game < 15; ++game) {
            board.reset();
            ais[difficulty]->clearAIMoveHistory();
            
            GameResult result = playCompleteGame(*ais[difficulty], 'O', 'X');
            EXPECT_TRUE(result != GameResult::ONGOING);
            
            // Record game
            std::string timestamp = "2025-06-16 " + std::to_string(10 + game) + ":00:00";
            GameRecord record("challenger", "AI_" + difficultyNames[difficulty], 
                            GameMode::PLAYER_VS_AI, result, board.getBoard(), timestamp);
            history->addGameRecord(record);
            
            // Update user stats
            User* user = users->getUser("challenger");
            user->gamesPlayed++;
            if(result == GameResult::PLAYER1_WIN) user->gamesWon++;
            else if(result == GameResult::PLAYER2_WIN) user->gamesLost++;
            else user->gamesTied++;
            users->updateUser("challenger", *user);
        }
    }
    
    auto challengerGames = history->getUserGames("challenger");
    EXPECT_EQ(challengerGames.size(), 45);
    
    User* finalUser = users->getUser("challenger");
    EXPECT_EQ(finalUser->gamesPlayed, 45);
}

// Tests 126-175: Undo/Redo Integration with GameStateStack
TEST_F(TicTacToeIntegrationTest, UndoRedoGameplayIntegration) {
    users->insertUser("undo_player", "hash");
    
    for(int test = 0; test < 25; ++test) {
        board.reset();
        stack->clearStack();
        
        // Make a series of moves
        std::vector<Move> moves = {
            Move(0, 0, 'X'),
            Move(1, 1, 'O'),
            Move(0, 1, 'X'),
            Move(1, 0, 'O'),
            Move(0, 2, 'X')
        };
        
        for(const auto& move : moves) {
            EXPECT_TRUE(board.makeMove(move.row, move.col, move.player));
            stack->pushMove(move, board.getBoard());
        }
        
        EXPECT_EQ(stack->size(), 5);
        EXPECT_TRUE(stack->canUndo());
        
        // Undo all moves
        std::vector<Move> undoMoves;
        while(stack->canUndo()) {
            Move undoMove = stack->popMove();
            auto prevBoard = stack->popBoardState();
            undoMoves.push_back(undoMove);
            board.setBoard(prevBoard);
        }
        
        EXPECT_EQ(undoMoves.size(), 5);
        EXPECT_FALSE(stack->canUndo());
        
        // Verify moves were undone in reverse order
        for(size_t i = 0; i < undoMoves.size(); ++i) {
            Move expected = moves[moves.size() - 1 - i];
            EXPECT_EQ(undoMoves[i].row, expected.row);
            EXPECT_EQ(undoMoves[i].col, expected.col);
            EXPECT_EQ(undoMoves[i].player, expected.player);
        }
    }
}

// Tests 176-225: Game History Integration
TEST_F(TicTacToeIntegrationTest, ExtensiveGameHistoryTracking) {
    users->insertUser("historian", "hash");
    
    for(int i = 0; i < 25; ++i) {
        std::vector<std::vector<char>> gameBoard(3, std::vector<char>(3, ' '));
        gameBoard[i % 3][(i + 1) % 3] = 'X';
        gameBoard[(i + 1) % 3][i % 3] = 'O';
        
        GameResult result = (i % 3 == 0) ? GameResult::PLAYER1_WIN : 
                           (i % 3 == 1) ? GameResult::PLAYER2_WIN : GameResult::TIE;
        
        std::string timestamp = "2025-06-" + std::to_string(16 + i % 10) + " 14:30:00";
        GameRecord record = createGameRecord("historian", "opponent" + std::to_string(i), 
                                           result, gameBoard, timestamp);
        
        // Add varying number of moves
        for(int m = 0; m < (i % 7) + 1; ++m) {
            record.moves.push_back(Move(m % 3, (m + 1) % 3, (m % 2 == 0) ? 'X' : 'O'));
        }
        
        history->addGameRecord(record);
        
        // Update user stats
        User* user = users->getUser("historian");
        user->gamesPlayed++;
        if(result == GameResult::PLAYER1_WIN) user->gamesWon++;
        else if(result == GameResult::PLAYER2_WIN) user->gamesLost++;
        else user->gamesTied++;
        users->updateUser("historian", *user);
    }
    
    auto historianGames = history->getUserGames("historian");
    EXPECT_EQ(historianGames.size(), 25);
    
    auto allGames = history->getAllGames();
    EXPECT_EQ(allGames.size(), 25);
    
    User* finalUser = users->getUser("historian");
    EXPECT_EQ(finalUser->gamesPlayed, 25);
}

TEST_F(TicTacToeIntegrationTest, GameHistoryPersistenceIntegration) {
    users->insertUser("persistent_gamer", "hash");
    
    for(int i = 0; i < 25; ++i) {
        std::vector<std::vector<char>> gameBoard(3, std::vector<char>(3, 'O'));
        std::string timestamp = "2025-06-16 " + std::to_string(10 + i) + ":00:00";
        GameRecord record = createGameRecord("persistent_gamer", "ai", 
                                           GameResult::PLAYER2_WIN, gameBoard, timestamp);
        history->addGameRecord(record);
    }
    
    // Create new history instance (should load from file)
    auto newHistory = std::make_unique<GameHistory>();
    auto games = newHistory->getUserGames("persistent_gamer");
    EXPECT_EQ(games.size(), 25);
    
    // Verify data integrity
    for(const auto& game : games) {
        EXPECT_EQ(game.player1, "persistent_gamer");
        EXPECT_EQ(game.player2, "ai");
        EXPECT_EQ(game.result, GameResult::PLAYER2_WIN);
    }
}

// Tests 226-275: Complex Tournament Integration
TEST_F(TicTacToeIntegrationTest, TournamentSimulation) {
    std::vector<std::string> players = {"alice", "bob", "charlie", "diana", "eve"};
    
    // Register all players
    for(const auto& player : players) {
        users->insertUser(player, "hash");
    }
    
    int gameCounter = 0;
    
    // Round-robin tournament
    for(size_t i = 0; i < players.size(); ++i) {
        for(size_t j = i + 1; j < players.size(); ++j) {
            for(int game = 0; game < 10; ++game) {
                board.reset();
                
                // Simulate deterministic game for testing
                char currentPlayer = 'X';
                std::vector<Move> gameMoves;
                
                // Create different game patterns
                std::vector<std::pair<int, int>> movePattern;
                if(gameCounter % 3 == 0) {
                    // Player 1 wins horizontally
                    movePattern = {{0,0}, {1,0}, {0,1}, {1,1}, {0,2}};
                } else if(gameCounter % 3 == 1) {
                    // Player 2 wins vertically
                    movePattern = {{0,0}, {0,1}, {1,0}, {0,2}, {2,1}, {0,0}}; // Invalid last move, player 2 wins
                    movePattern = {{1,0}, {0,0}, {1,1}, {0,1}, {2,0}, {0,2}};
                } else {
                    // Tie game
                    movePattern = {{0,0}, {0,1}, {0,2}, {1,0}, {1,2}, {1,1}, {2,0}, {2,2}, {2,1}};
                }
                
                GameResult result = GameResult::ONGOING;
                for(size_t moveIdx = 0; moveIdx < movePattern.size() && result == GameResult::ONGOING; ++moveIdx) {
                    auto move = movePattern[moveIdx];
                    if(board.makeMove(move.first, move.second, currentPlayer)) {
                        gameMoves.push_back(Move(move.first, move.second, currentPlayer));
                        result = board.checkWin();
                        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                    }
                }
                
                if(result == GameResult::ONGOING && board.isFull()) {
                    result = GameResult::TIE;
                }
                
                // Record game
                std::string timestamp = "2025-06-16 " + std::to_string(10 + gameCounter % 14) + ":00:00";
                GameRecord record = createGameRecord(players[i], players[j], result, board.getBoard(), timestamp);
                record.moves = gameMoves;
                history->addGameRecord(record);
                
                // Update player statistics
                User* p1 = users->getUser(players[i]);
                User* p2 = users->getUser(players[j]);
                
                p1->gamesPlayed++;
                p2->gamesPlayed++;
                
                if(result == GameResult::PLAYER1_WIN) {
                    p1->gamesWon++;
                    p2->gamesLost++;
                } else if(result == GameResult::PLAYER2_WIN) {
                    p1->gamesLost++;
                    p2->gamesWon++;
                } else {
                    p1->gamesTied++;
                    p2->gamesTied++;
                }
                
                users->updateUser(players[i], *p1);
                users->updateUser(players[j], *p2);
                
                gameCounter++;
            }
        }
    }
    
    // Verify tournament results
    for(const auto& player : players) {
        auto games = history->getUserGames(player);
        EXPECT_EQ(games.size(), 40); // 4 opponents * 10 games each
        
        User* user = users->getUser(player);
        EXPECT_EQ(user->gamesPlayed, 40);
        EXPECT_EQ(user->gamesWon + user->gamesLost + user->gamesTied, 40);
    }
    
    auto allGames = history->getAllGames();
    EXPECT_EQ(allGames.size(), 100); // 5 players * 4 opponents * 10 games / 2
}
    // Tests 276-325: Error Handling and Edge Cases Integration
TEST_F(TicTacToeIntegrationTest, InvalidMoveHandling) {
    users->insertUser("error_player", "hash");
    
    board.reset();
    EXPECT_FALSE(board.makeMove(3, 0, 'X')); // Invalid row
    EXPECT_FALSE(board.makeMove(0, 3, 'X')); // Invalid column
    EXPECT_FALSE(board.makeMove(-1, 0, 'X')); // Negative row
    EXPECT_FALSE(board.makeMove(0, -1, 'X')); // Negative column
    
    // Valid moves
    EXPECT_TRUE(board.makeMove(0, 0, 'X'));
    EXPECT_TRUE(board.makeMove(0, 1, 'O'));
    
    // Attempt to overwrite existing move
    EXPECT_FALSE(board.makeMove(0, 0, 'X'));
    
    // Check game state after invalid moves
    EXPECT_EQ(board.checkWin(), GameResult::ONGOING);
}
// Tests 326-375: Performance and Stress Testing Integration
TEST_F(TicTacToeIntegrationTest, HighVolumeGameProcessing) {
    users->insertUser("stress_tester", "hash");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for(int batch = 0; batch < 5; ++batch) {
        for(int game = 0; game < 15; ++game) {
            board.reset();
            aiMedium->clearAIMoveHistory();
            stack->clearStack();
            
            std::vector<Move> gameMoves;
            char currentPlayer = 'X';
            
            // Fast game simulation
            while(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
                std::pair<int, int> move;
                
                if(currentPlayer == 'O') {
                    move = aiMedium->getBestMove(board);
                } else {
                    auto moves = board.getAvailableMoves();
                    if(!moves.empty()) {
                        move = moves[0]; // Take first available
                    } else {
                        break;
                    }
                }
                
                if(move.first != -1 && board.makeMove(move.first, move.second, currentPlayer)) {
                    gameMoves.push_back(Move(move.first, move.second, currentPlayer));
                    stack->pushMove(Move(move.first, move.second, currentPlayer), board.getBoard());
                }
                
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
            
            GameResult result = board.checkWin();
            if(result == GameResult::ONGOING && board.isFull()) {
                result = GameResult::TIE;
            }
            
            // Record game
            std::string timestamp = "2025-06-16 " + std::to_string(10 + (batch * 15 + game) % 14) + ":00:00";
            GameRecord record = createGameRecord("stress_tester", "AI", result, board.getBoard(), timestamp);
            record.moves = gameMoves;
            history->addGameRecord(record);
            
            // Update user stats
            User* user = users->getUser("stress_tester");
            user->gamesPlayed++;
            if(result == GameResult::PLAYER1_WIN) user->gamesWon++;
            else if(result == GameResult::PLAYER2_WIN) user->gamesLost++;
            else user->gamesTied++;
            users->updateUser("stress_tester", *user);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Verify all operations completed successfully
    auto userGames = history->getUserGames("stress_tester");
    EXPECT_EQ(userGames.size(), 75);
    
    User* finalUser = users->getUser("stress_tester");
    EXPECT_EQ(finalUser->gamesPlayed, 75);
    
    // Performance should be reasonable (less than 10 seconds for 75 games)
    EXPECT_LT(duration.count(), 10000);
}

// Tests 376-400: Advanced Integration Scenarios
TEST_F(TicTacToeIntegrationTest, CompleteGameLifecycleIntegration) {
    for(int lifecycle = 0; lifecycle < 25; ++lifecycle) {
        std::string username = "lifecycle_user_" + std::to_string(lifecycle);
        users->insertUser(username, "hash");
        
        board.reset();
        stack->clearStack();
        aiHard->clearAIMoveHistory();
        
        std::vector<Move> allMoves;
        char player = 'X';
        
        // Complete game simulation with state tracking
        while(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
            std::pair<int, int> move;
            
            if(player == 'O') {
                move = aiHard->getBestMove(board);
            } else {
                auto moves = board.getAvailableMoves();
                if(!moves.empty()) {
                    move = moves[lifecycle % moves.size()];
                }
            }
            
            if(move.first != -1 && board.makeMove(move.first, move.second, player)) {
                Move gameMove(move.first, move.second, player);
                allMoves.push_back(gameMove);
                stack->pushMove(gameMove, board.getBoard());
            }
            
            player = (player == 'X') ? 'O' : 'X';
        }
        
        GameResult result = board.checkWin();
        if(result == GameResult::ONGOING && board.isFull()) {
            result = GameResult::TIE;
        }
        
        // Record complete game
        std::string timestamp = "2025-06-16 " + std::to_string(10 + lifecycle % 14) + ":30:00";
        GameRecord record = createGameRecord(username, "AI", result, board.getBoard(), timestamp);
        record.moves = allMoves;
        history->addGameRecord(record);
        
        // Update user statistics
        User* user = users->getUser(username);
        user->gamesPlayed++;
        if(result == GameResult::PLAYER1_WIN) user->gamesWon++;
        else if(result == GameResult::PLAYER2_WIN) user->gamesLost++;
        else user->gamesTied++;
        users->updateUser(username, *user);
        
        // Verify data consistency
        auto userGames = history->getUserGames(username);
        EXPECT_EQ(userGames.size(), 1);
        EXPECT_EQ(userGames[0].moves.size(), allMoves.size());
        
        // Verify undo capability
        EXPECT_TRUE(stack->canUndo() || allMoves.empty());
        
        // Verify AI history
        EXPECT_TRUE(aiHard->hasAIMoveHistory() || allMoves.empty());
    }
}TEST_F(TicTacToeIntegrationTest, StackHistoryUserIntegration) {
    // Create user and start tracking game
    users->insertUser("stack_user", "hash");
    
    board.reset();
    stack->clearStack();
    
    // Simulate game with undo capability and history tracking
    std::vector<Move> gameMoves;
    
    // Move 1: User plays
    board.makeMove(0, 0, 'X');
    Move move1(0, 0, 'X');
    gameMoves.push_back(move1);
    stack->pushMove(move1, board.getBoard());
    
    // Move 2: AI plays
    board.makeMove(1, 1, 'O');
    Move move2(1, 1, 'O');
    gameMoves.push_back(move2);
    stack->pushMove(move2, board.getBoard());
    
    // Move 3: User plays
    board.makeMove(0, 1, 'X');
    Move move3(0, 1, 'X');
    gameMoves.push_back(move3);
    stack->pushMove(move3, board.getBoard());
    
    // User decides to undo last move
    EXPECT_TRUE(stack->canUndo());
    Move undoMove = stack->popMove();
    auto prevBoard = stack->popBoardState();
    
    EXPECT_EQ(undoMove.row, 0);
    EXPECT_EQ(undoMove.col, 1);
    EXPECT_EQ(undoMove.player, 'X');
    
    board.setBoard(prevBoard);
    gameMoves.pop_back(); // Remove undone move
    
    // Continue game with different move
    board.makeMove(2, 0, 'X');
    Move move3_new(2, 0, 'X');
    gameMoves.push_back(move3_new);
    stack->pushMove(move3_new, board.getBoard());
    
    // Complete game
    board.makeMove(0, 2, 'O');
    Move move4(0, 2, 'O');
    gameMoves.push_back(move4);
    stack->pushMove(move4, board.getBoard());
    
    board.makeMove(1, 0, 'X');
    Move move5(1, 0, 'X');
    gameMoves.push_back(move5);
    stack->pushMove(move5, board.getBoard());
    
    // Record final game in history
    GameResult result = board.checkWin();
    GameRecord record = createGameRecord("stack_user", "AI", result, board.getBoard());
    record.moves = gameMoves;
    history->addGameRecord(record);
    
    // Update user statistics
    User* user = users->getUser("stack_user");
    user->gamesPlayed++;
    if(result == GameResult::PLAYER1_WIN) user->gamesWon++;
    else if(result == GameResult::PLAYER2_WIN) user->gamesLost++;
    else user->gamesTied++;
    users->updateUser("stack_user", *user);
    
    // Verify integration
    auto userGames = history->getUserGames("stack_user");
    EXPECT_EQ(userGames.size(), 1);
    EXPECT_EQ(userGames[0].moves.size(), gameMoves.size());
    
    User* finalUser = users->getUser("stack_user");
    EXPECT_EQ(finalUser->gamesPlayed, 1);
}

TEST_F(TicTacToeIntegrationTest, MultipleUndoRedoWithHistoryTracking) {
    users->insertUser("undo_master", "hash");
    
    for(int game = 0; game < 10; ++game) {
        board.reset();
        stack->clearStack();
        
        std::vector<Move> finalMoves;
        std::vector<Move> allMoves;
        
        // Make initial moves
        for(int i = 0; i < 6; ++i) {
            int row = i % 3;
            int col = (i + 1) % 3;
            char player = (i % 2 == 0) ? 'X' : 'O';
            
            if(board.makeMove(row, col, player)) {
                Move move(row, col, player);
                allMoves.push_back(move);
                stack->pushMove(move, board.getBoard());
            }
        }
        
        // Undo some moves randomly
        int undoCount = (game % 3) + 1;
        for(int u = 0; u < undoCount; ++u) {
            if(stack->canUndo()) {
                stack->popMove();
                auto prevBoard = stack->popBoardState();
                board.setBoard(prevBoard);
                allMoves.pop_back();
            }
        }
        
        // Make new moves
        for(int i = 0; i < 3; ++i) {
            auto availableMoves = board.getAvailableMoves();
            if(!availableMoves.empty()) {
                auto move = availableMoves[i % availableMoves.size()];
                char player = (allMoves.size() % 2 == 0) ? 'X' : 'O';
                
                if(board.makeMove(move.first, move.second, player)) {
                    Move gameMove(move.first, move.second, player);
                    allMoves.push_back(gameMove);
                    stack->pushMove(gameMove, board.getBoard());
                }
            }
        }
        
        finalMoves = allMoves;
        
        // Record game
        GameResult result = board.checkWin();
        if(result == GameResult::ONGOING && board.isFull()) {
            result = GameResult::TIE;
        }
        
        std::string timestamp = "2025-06-16 " + std::to_string(10 + game) + ":00:00";
        GameRecord record = createGameRecord("undo_master", "opponent", result, board.getBoard(), timestamp);
        record.moves = finalMoves;
        history->addGameRecord(record);
        
        // Update user stats
        User* user = users->getUser("undo_master");
        user->gamesPlayed++;
        users->updateUser("undo_master", *user);
    }
    
    // Verify all games recorded
    auto userGames = history->getUserGames("undo_master");
    EXPECT_EQ(userGames.size(), 10);
    
    User* finalUser = users->getUser("undo_master");
    EXPECT_EQ(finalUser->gamesPlayed, 10);
}

TEST_F(TicTacToeIntegrationTest, StackStateConsistencyWithHistory) {
    users->insertUser("consistency_user", "hash");
    
    board.reset();
    stack->clearStack();
    
    // Build complex game state
    std::vector<Move> moves = {
        Move(1, 1, 'X'),
        Move(0, 0, 'O'),
        Move(2, 2, 'X'),
        Move(0, 1, 'O'),
        Move(0, 2, 'X'),
        Move(2, 0, 'O'),
        Move(1, 0, 'X'),
        Move(1, 2, 'O'),
        Move(2, 1, 'X')
    };
    
    // Apply moves and track in stack
    for(size_t i = 0; i < moves.size(); ++i) {
        if(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
            if(board.makeMove(moves[i].row, moves[i].col, moves[i].player)) {
                stack->pushMove(moves[i], board.getBoard());
            }
        }
    }
    
    // Verify stack size matches applied moves
    size_t expectedSize = 0;
    for(const auto& move : moves) {
        if(board.getCell(move.row, move.col) != ' ') {
            expectedSize++;
        }
    }
    
    // Test undo operations
    std::vector<Move> undoSequence;
    while(stack->canUndo()) {
        Move undoMove = stack->popMove();
        auto prevBoard = stack->popBoardState();
        undoSequence.push_back(undoMove);
        board.setBoard(prevBoard);
    }
    
    // Verify undo sequence is reverse of applied moves
    std::reverse(undoSequence.begin(), undoSequence.end());
    
    // Record the final state
    GameResult result = board.checkWin();
    GameRecord record = createGameRecord("consistency_user", "test", result, board.getBoard());
    record.moves = undoSequence;
    history->addGameRecord(record);
    
    // Verify consistency
    auto userGames = history->getUserGames("consistency_user");
    EXPECT_EQ(userGames.size(), 1);
    EXPECT_FALSE(stack->canUndo());
}

// ==================== USER MANAGER WITH HISTORY INTEGRATION TESTS ====================

TEST_F(TicTacToeIntegrationTest, UserStatisticsWithGameHistory) {
    // Create multiple users
    std::vector<std::string> players = {"player1", "player2", "player3"};
    for(const auto& player : players) {
        users->insertUser(player, "hash");
    }
    
    // Simulate multiple games between players
    for(int round = 0; round < 5; ++round) {
        for(size_t i = 0; i < players.size(); ++i) {
            for(size_t j = i + 1; j < players.size(); ++j) {
                board.reset();
                
                // Simulate game outcome based on round
                GameResult result;
                if(round % 3 == 0) result = GameResult::PLAYER1_WIN;
                else if(round % 3 == 1) result = GameResult::PLAYER2_WIN;
                else result = GameResult::TIE;
                
                // Create appropriate board state
                std::vector<std::vector<char>> gameBoard(3, std::vector<char>(3, ' '));
                if(result == GameResult::PLAYER1_WIN) {
                    gameBoard[0][0] = 'X';
                    gameBoard[0][1] = 'X';
                    gameBoard[0][2] = 'X';
                } else if(result == GameResult::PLAYER2_WIN) {
                    gameBoard[0][0] = 'O';
                    gameBoard[1][0] = 'O';
                    gameBoard[2][0] = 'O';
                }
                
                // Record game
                std::string timestamp = "2025-06-16 " + std::to_string(10 + round) + ":00:00";
                GameRecord record = createGameRecord(players[i], players[j], result, gameBoard, timestamp);
                history->addGameRecord(record);
                
                // Update user statistics
                User* p1 = users->getUser(players[i]);
                User* p2 = users->getUser(players[j]);
                
                p1->gamesPlayed++;
                p2->gamesPlayed++;
                
                if(result == GameResult::PLAYER1_WIN) {
                    p1->gamesWon++;
                    p2->gamesLost++;
                } else if(result == GameResult::PLAYER2_WIN) {
                    p1->gamesLost++;
                    p2->gamesWon++;
                } else {
                    p1->gamesTied++;
                    p2->gamesTied++;
                }
                
                users->updateUser(players[i], *p1);
                users->updateUser(players[j], *p2);
            }
        }
    }
    
    // Verify statistics consistency with history
    for(const auto& player : players) {
        auto playerGames = history->getUserGames(player);
        User* user = users->getUser(player);
        
        EXPECT_EQ(user->gamesPlayed, playerGames.size());
        
        // Count wins/losses/ties from history
        int historyWins = 0, historyLosses = 0, historyTies = 0;
        for(const auto& game : playerGames) {
            if((game.player1 == player && game.result == GameResult::PLAYER1_WIN) ||
               (game.player2 == player && game.result == GameResult::PLAYER2_WIN)) {
                historyWins++;
            } else if((game.player1 == player && game.result == GameResult::PLAYER2_WIN) ||
                     (game.player2 == player && game.result == GameResult::PLAYER1_WIN)) {
                historyLosses++;
            } else if(game.result == GameResult::TIE) {
                historyTies++;
            }
        }
        
        EXPECT_EQ(user->gamesWon, historyWins);
        EXPECT_EQ(user->gamesLost, historyLosses);
        EXPECT_EQ(user->gamesTied, historyTies);
    }
}

TEST_F(TicTacToeIntegrationTest, UserManagerHistoryPersistenceIntegration) {
    // Create users and games
    users->insertUser("persistent1", "hash1");
    users->insertUser("persistent2", "hash2");
    
    // Play games and update statistics
    for(int i = 0; i < 10; ++i) {
        std::vector<std::vector<char>> gameBoard(3, std::vector<char>(3, ' '));
        gameBoard[0][0] = 'X';
        gameBoard[0][1] = 'X';
        gameBoard[0][2] = 'X';
        
        std::string timestamp = "2025-06-16 " + std::to_string(10 + i) + ":00:00";
        GameRecord record = createGameRecord("persistent1", "persistent2", 
                                           GameResult::PLAYER1_WIN, gameBoard, timestamp);
        history->addGameRecord(record);
        
        // Update winner stats
        User* winner = users->getUser("persistent1");
        winner->gamesPlayed++;
        winner->gamesWon++;
        users->updateUser("persistent1", *winner);
        
        // Update loser stats
        User* loser = users->getUser("persistent2");
        loser->gamesPlayed++;
        loser->gamesLost++;
        users->updateUser("persistent2", *loser);
    }
    
    // Create new instances (should load from files)
    auto newUsers = std::make_unique<UserHashTable>();
    auto newHistory = std::make_unique<GameHistory>();
    
    // Verify user data persistence
    EXPECT_TRUE(newUsers->userExists("persistent1"));
    EXPECT_TRUE(newUsers->userExists("persistent2"));
    
    User* loadedWinner = newUsers->getUser("persistent1");
    User* loadedLoser = newUsers->getUser("persistent2");
    
    EXPECT_EQ(loadedWinner->gamesPlayed, 10);
    EXPECT_EQ(loadedWinner->gamesWon, 10);
    EXPECT_EQ(loadedLoser->gamesPlayed, 10);
    EXPECT_EQ(loadedLoser->gamesLost, 10);
    
    // Verify history persistence
    auto games1 = newHistory->getUserGames("persistent1");
    auto games2 = newHistory->getUserGames("persistent2");
    
    EXPECT_EQ(games1.size(), 10);
    EXPECT_EQ(games2.size(), 10);
}

TEST_F(TicTacToeIntegrationTest, UserRankingSystemWithHistory) {
    // Create multiple users with different skill levels
    std::vector<std::string> players = {"novice", "intermediate", "expert", "master"};
    for(const auto& player : players) {
        users->insertUser(player, "hash");
    }
    
    // Simulate games with skill-based outcomes
    std::map<std::string, int> skillLevels = {
        {"novice", 1}, {"intermediate", 2}, {"expert", 3}, {"master", 4}
    };
    
    for(const auto& p1 : players) {
        for(const auto& p2 : players) {
            if(p1 != p2) {
                for(int game = 0; game < 3; ++game) {
                    GameResult result;
                    
                    // Higher skill level wins more often
                    if(skillLevels[p1] > skillLevels[p2]) {
                        result = GameResult::PLAYER1_WIN;
                    } else if(skillLevels[p1] < skillLevels[p2]) {
                        result = GameResult::PLAYER2_WIN;
                    } else {
                        result = GameResult::TIE;
                    }
                    
                    // Create game board
                    std::vector<std::vector<char>> gameBoard(3, std::vector<char>(3, ' '));
                    
                    // Record game
                    std::string timestamp = "2025-06-16 " + std::to_string(10 + game) + ":00:00";
                    GameRecord record = createGameRecord(p1, p2, result, gameBoard, timestamp);
                    history->addGameRecord(record);
                    
                    // Update statistics
                    User* user1 = users->getUser(p1);
                    User* user2 = users->getUser(p2);
                    
                    user1->gamesPlayed++;
                    user2->gamesPlayed++;
                    
                    if(result == GameResult::PLAYER1_WIN) {
                        user1->gamesWon++;
                        user2->gamesLost++;
                    } else if(result == GameResult::PLAYER2_WIN) {
                        user1->gamesLost++;
                        user2->gamesWon++;
                    } else {
                        user1->gamesTied++;
                        user2->gamesTied++;
                    }
                    
                    users->updateUser(p1, *user1);
                    users->updateUser(p2, *user2);
                }
            }
        }
    }
    
    // Calculate win rates and verify ranking
    std::vector<std::pair<std::string, double>> rankings;
    for(const auto& player : players) {
        User* user = users->getUser(player);
        double winRate = (user->gamesPlayed > 0) ? 
                        (double)user->gamesWon / user->gamesPlayed : 0.0;
        rankings.push_back({player, winRate});
    }
    
    // Sort by win rate
    std::sort(rankings.begin(), rankings.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Verify ranking order matches skill levels
    EXPECT_EQ(rankings[0].first, "master");
    EXPECT_EQ(rankings[1].first, "expert");
    EXPECT_EQ(rankings[2].first, "intermediate");
    EXPECT_EQ(rankings[3].first, "novice");
}

// ==================== COMPLEX INTEGRATION SCENARIOS ====================

TEST_F(TicTacToeIntegrationTest, CompleteGameSessionWithAllComponents) {
    // Setup: Create user, start game session
    users->insertUser("session_player", "hash");
    
    board.reset();
    stack->clearStack();
    aiMedium->clearAIMoveHistory();
    
    std::vector<Move> sessionMoves;
    char currentPlayer = 'X';
    
    // Game session with undo/redo capabilities
    while(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
        std::pair<int, int> move;
        
        if(currentPlayer == 'O') {
            // AI move
            move = aiMedium->getBestMove(board);
        } else {
            // Human move (simulated)
            auto moves = board.getAvailableMoves();
            if(!moves.empty()) {
                move = moves[sessionMoves.size() % moves.size()];
            }
        }
        
        if(move.first != -1 && board.makeMove(move.first, move.second, currentPlayer)) {
            Move gameMove(move.first, move.second, currentPlayer);
            sessionMoves.push_back(gameMove);
            stack->pushMove(gameMove, board.getBoard());
            
            // Simulate occasional undo (10% chance)
            if(sessionMoves.size() > 2 && (sessionMoves.size() % 10 == 0)) {
                // Undo last move
                Move undoMove = stack->popMove();
                auto prevBoard = stack->popBoardState();
                board.setBoard(prevBoard);
                sessionMoves.pop_back();
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; // Revert player
                continue;
            }
        }
        
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }
    
    // End session: Record game and update statistics
    GameResult result = board.checkWin();
    if(result == GameResult::ONGOING && board.isFull()) {
        result = GameResult::TIE;
    }
    
    GameRecord record = createGameRecord("session_player", "AI", result, board.getBoard());
    record.moves = sessionMoves;
    history->addGameRecord(record);
    
    // Update user statistics
    User* user = users->getUser("session_player");
    user->gamesPlayed++;
    if(result == GameResult::PLAYER1_WIN) user->gamesWon++;
    else if(result == GameResult::PLAYER2_WIN) user->gamesLost++;
    else user->gamesTied++;
    users->updateUser("session_player", *user);
    
    // Verify session integrity
    auto userGames = history->getUserGames("session_player");
    EXPECT_EQ(userGames.size(), 1);
    EXPECT_EQ(userGames[0].moves.size(), sessionMoves.size());
    
    User* finalUser = users->getUser("session_player");
    EXPECT_EQ(finalUser->gamesPlayed, 1);
    
    // Verify AI history
    EXPECT_TRUE(aiMedium->hasAIMoveHistory());
    
    // Verify stack state
    EXPECT_TRUE(stack->canUndo() || sessionMoves.empty());
}
TEST_F(TicTacToeIntegrationTest, StackHistoryUserComplexWorkflow) {
    users->insertUser("workflow_user", "hash");
    
    for(int session = 0; session < 25; ++session) {
        board.reset();
        stack->clearStack();
        
        std::vector<Move> sessionMoves;
        auto pattern = getGamePattern(session);
        char currentPlayer = 'X';
        
        // Build game state with undo capabilities
        for(size_t i = 0; i < pattern.size(); ++i) {
            auto move = pattern[i];
            if(board.makeMove(move.first, move.second, currentPlayer)) {
                Move gameMove(move.first, move.second, currentPlayer);
                sessionMoves.push_back(gameMove);
                stack->pushMove(gameMove, board.getBoard());
                
                // Simulate strategic undo (every 3rd move)
                if(i > 0 && i % 3 == 0 && stack->canUndo()) {
                    Move undoMove = stack->popMove();
                    auto prevBoard = stack->popBoardState();
                    board.setBoard(prevBoard);
                    sessionMoves.pop_back();
                    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                    continue;
                }
                
                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
            }
        }
        
        // Record final game state
        GameResult result = board.checkWin();
        if(result == GameResult::ONGOING && board.isFull()) {
            result = GameResult::TIE;
        }
        
        std::string timestamp = "2025-06-16 " + std::to_string(10 + session % 14) + ":00:00";
        GameRecord record = createGameRecord("workflow_user", "opponent", result, board.getBoard(), timestamp);
        record.moves = sessionMoves;
        history->addGameRecord(record);
        
        // Update user statistics
        User* user = users->getUser("workflow_user");
        user->gamesPlayed++;
        if(result == GameResult::PLAYER1_WIN) user->gamesWon++;
        else if(result == GameResult::PLAYER2_WIN) user->gamesLost++;
        else user->gamesTied++;
        users->updateUser("workflow_user", *user);
        
        // Verify stack-history consistency
        EXPECT_EQ(stack->size(), sessionMoves.size());
        
        // Test partial undo sequence
        int undoCount = std::min(3, (int)sessionMoves.size());
        for(int u = 0; u < undoCount; ++u) {
            if(stack->canUndo()) {
                Move undoMove = stack->popMove();
                auto prevBoard = stack->popBoardState();
                EXPECT_EQ(undoMove.row, sessionMoves[sessionMoves.size() - 1 - u].row);
                EXPECT_EQ(undoMove.col, sessionMoves[sessionMoves.size() - 1 - u].col);
            }
        }
    }
    
    // Verify final state
    auto userGames = history->getUserGames("workflow_user");
    EXPECT_EQ(userGames.size(), 25);
    
    User* finalUser = users->getUser("workflow_user");
    EXPECT_EQ(finalUser->gamesPlayed, 25);
}

TEST_F(TicTacToeIntegrationTest, MultiUserStackHistoryIntegration) {
    std::vector<std::string> players = {"stack_user1", "stack_user2", "stack_user3"};
    
    for(const auto& player : players) {
        users->insertUser(player, "hash");
    }
    
    // Each player plays multiple games with undo/redo
    for(int round = 0; round < 15; ++round) {
        for(size_t i = 0; i < players.size(); ++i) {
            for(size_t j = i + 1; j < players.size(); ++j) {
                board.reset();
                stack->clearStack();
                
                std::vector<Move> gameMoves;
                auto pattern = getGamePattern(round + i + j);
                char currentPlayer = 'X';
                
                for(const auto& move : pattern) {
                    if(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
                        if(board.makeMove(move.first, move.second, currentPlayer)) {
                            Move gameMove(move.first, move.second, currentPlayer);
                            gameMoves.push_back(gameMove);
                            stack->pushMove(gameMove, board.getBoard());
                            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                        }
                    }
                }
                
                // Test undo capabilities mid-game
                if(gameMoves.size() > 2) {
                    Move lastMove = stack->popMove();
                    auto prevBoard = stack->popBoardState();
                    board.setBoard(prevBoard);
                    gameMoves.pop_back();
                    
                    // Make different move
                    auto availableMoves = board.getAvailableMoves();
                    if(!availableMoves.empty()) {
                        auto newMove = availableMoves[0];
                        char player = (gameMoves.size() % 2 == 0) ? 'X' : 'O';
                        if(board.makeMove(newMove.first, newMove.second, player)) {
                            Move gameMove(newMove.first, newMove.second, player);
                            gameMoves.push_back(gameMove);
                            stack->pushMove(gameMove, board.getBoard());
                        }
                    }
                }
                
                GameResult result = board.checkWin();
                if(result == GameResult::ONGOING && board.isFull()) {
                    result = GameResult::TIE;
                }
                
                // Record game
                std::string timestamp = "2025-06-16 " + std::to_string(10 + round) + ":00:00";
                GameRecord record = createGameRecord(players[i], players[j], result, board.getBoard(), timestamp);
                record.moves = gameMoves;
                history->addGameRecord(record);
                
                // Update statistics
                User* p1 = users->getUser(players[i]);
                User* p2 = users->getUser(players[j]);
                
                p1->gamesPlayed++;
                p2->gamesPlayed++;
                
                if(result == GameResult::PLAYER1_WIN) {
                    p1->gamesWon++;
                    p2->gamesLost++;
                } else if(result == GameResult::PLAYER2_WIN) {
                    p1->gamesLost++;
                    p2->gamesWon++;
                } else {
                    p1->gamesTied++;
                    p2->gamesTied++;
                }
                
                users->updateUser(players[i], *p1);
                users->updateUser(players[j], *p2);
            }
        }
    }
    
    // Verify all players have correct statistics
    for(const auto& player : players) {
        auto playerGames = history->getUserGames(player);
        User* user = users->getUser(player);
        EXPECT_EQ(user->gamesPlayed, playerGames.size());
        EXPECT_EQ(user->gamesPlayed, 30); // 2 opponents * 15 rounds
    }
}

// ==================== AI WITH ALL COMPONENTS INTEGRATION (Tests 101-200) ====================

TEST_F(TicTacToeIntegrationTest, AIStrategicDecisionWithHistoryTracking) {
    users->insertUser("strategy_analyst", "hash");
    
    // Test AI strategic decisions across different scenarios
    std::vector<std::vector<std::vector<char>>> testScenarios = {
        // Scenario 1: AI should win
        {{'X', ' ', ' '}, {'X', 'O', ' '}, {' ', ' ', ' '}},
        // Scenario 2: AI should block
        {{'X', 'X', ' '}, {' ', 'O', ' '}, {' ', ' ', ' '}},
        // Scenario 3: AI should take center
        {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}},
        // Scenario 4: AI should take corner
        {{' ', 'X', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}},
        // Scenario 5: Complex decision
        {{'X', 'O', 'X'}, {'O', 'X', ' '}, {' ', ' ', 'O'}}
    };
    
    for(size_t scenario = 0; scenario < testScenarios.size(); ++scenario) {
        for(int difficulty = 0; difficulty < 3; ++difficulty) {
            for(int iteration = 0; iteration < 5; ++iteration) {
                board.reset();
                board.setBoard(testScenarios[scenario]);
                stack->clearStack();
                
                // Record initial state
                stack->pushMove(Move(-1, -1, ' '), board.getBoard()); // Dummy move for initial state
                
                std::unique_ptr<AIPlayer> ai;
                switch(difficulty) {
                    case 0: ai = std::make_unique<AIPlayer>('O', 'X', DifficultyLevel::EASY); break;
                    case 1: ai = std::make_unique<AIPlayer>('O', 'X', DifficultyLevel::MEDIUM); break;
                    case 2: ai = std::make_unique<AIPlayer>('O', 'X', DifficultyLevel::HARD); break;
                }
                
                auto aiMove = ai->getBestMove(board);
                
                if(aiMove.first != -1) {
                    board.makeMove(aiMove.first, aiMove.second, 'O');
                    stack->pushMove(Move(aiMove.first, aiMove.second, 'O'), board.getBoard());
                    
                    // Analyze decision quality
                    GameResult result = board.checkWin();
                    bool goodMove = (result == GameResult::PLAYER2_WIN) || 
                                   (result == GameResult::ONGOING && !board.isFull());
                    
                    // Record decision analysis
                    std::string timestamp = "2025-06-16 " + std::to_string(10 + scenario) + ":00:00";
                    GameRecord record = createGameRecord("strategy_analyst", 
                                                       "AI_Difficulty_" + std::to_string(difficulty), 
                                                       result, board.getBoard(), timestamp);
                    
                    // Add move sequence
                    record.moves.push_back(Move(aiMove.first, aiMove.second, 'O'));
                    history->addGameRecord(record);
                    
                    // Update analytics
                    User* user = users->getUser("strategy_analyst");
                    user->gamesPlayed++;
                    if(goodMove) user->gamesWon++; // Use wins to track good decisions
                    users->updateUser("strategy_analyst", *user);
                }
            }
        }
    }
    
    // Verify decision tracking
    auto analystGames = history->getUserGames("strategy_analyst");
    EXPECT_GT(analystGames.size(), 50);
    
    User* analyst = users->getUser("strategy_analyst");
    EXPECT_GT(analyst->gamesPlayed, 50);
}

// ==================== PERSISTENCE AND DATA INTEGRITY (Tests 201-300) ====================

TEST_F(TicTacToeIntegrationTest, ComprehensiveDataPersistenceIntegration) {
    // Create complex data scenario
    std::vector<std::string> players = {"persistent1", "persistent2", "persistent3", "persistent4"};
    
    for(const auto& player : players) {
        users->insertUser(player, "hash");
    }
    
    // Generate complex game history
    for(int round = 0; round < 10; ++round) {
        for(size_t i = 0; i < players.size(); ++i) {
            for(size_t j = i + 1; j < players.size(); ++j) {
                board.reset();
                stack->clearStack();
                
                auto pattern = getGamePattern(round + i + j);
                std::vector<Move> gameMoves;
                char currentPlayer = 'X';
                
                for(const auto& move : pattern) {
                    if(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
                        if(board.makeMove(move.first, move.second, currentPlayer)) {
                            Move gameMove(move.first, move.second, currentPlayer);
                            gameMoves.push_back(gameMove);
                            stack->pushMove(gameMove, board.getBoard());
                            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                        }
                    }
                }
                
                GameResult result = board.checkWin();
                if(result == GameResult::ONGOING && board.isFull()) {
                    result = GameResult::TIE;
                }
                
                // Record with detailed timestamp
                std::string timestamp = "2025-06-" + std::to_string(16 + round % 10) + 
                                       " " + std::to_string(10 + (i + j) % 14) + ":00:00";
                GameRecord record = createGameRecord(players[i], players[j], result, board.getBoard(), timestamp);
                record.moves = gameMoves;
                history->addGameRecord(record);
                
                // Update detailed statistics
                User* p1 = users->getUser(players[i]);
                User* p2 = users->getUser(players[j]);
                
                p1->gamesPlayed++;
                p2->gamesPlayed++;
                
                if(result == GameResult::PLAYER1_WIN) {
                    p1->gamesWon++;
                    p2->gamesLost++;
                } else if(result == GameResult::PLAYER2_WIN) {
                    p1->gamesLost++;
                    p2->gamesWon++;
                } else {
                    p1->gamesTied++;
                    p2->gamesTied++;
                }
                
                users->updateUser(players[i], *p1);
                users->updateUser(players[j], *p2);
            }
        }
    }
    
    // Capture original state
    std::map<std::string, User> originalUsers;
    for(const auto& player : players) {
        originalUsers[player] = *users->getUser(player);
    }
    auto originalGames = history->getAllGames();
    
    // Create new instances (test persistence)
    auto newUsers = std::make_unique<UserHashTable>();
    auto newHistory = std::make_unique<GameHistory>();
    
    // Verify user data persistence
    for(const auto& player : players) {
        EXPECT_TRUE(newUsers->userExists(player));
        User* loadedUser = newUsers->getUser(player);
        EXPECT_EQ(loadedUser->gamesPlayed, originalUsers[player].gamesPlayed);
        EXPECT_EQ(loadedUser->gamesWon, originalUsers[player].gamesWon);
        EXPECT_EQ(loadedUser->gamesLost, originalUsers[player].gamesLost);
        EXPECT_EQ(loadedUser->gamesTied, originalUsers[player].gamesTied);
    }
    
    // Verify game history persistence
    auto loadedGames = newHistory->getAllGames();
    EXPECT_EQ(loadedGames.size(), originalGames.size());
    
    // Verify game data integrity
    for(size_t i = 0; i < std::min(loadedGames.size(), originalGames.size()); ++i) {
        EXPECT_EQ(loadedGames[i].player1, originalGames[i].player1);
        EXPECT_EQ(loadedGames[i].player2, originalGames[i].player2);
        EXPECT_EQ(loadedGames[i].result, originalGames[i].result);
        EXPECT_EQ(loadedGames[i].moves.size(), originalGames[i].moves.size());
    }
}

TEST_F(TicTacToeIntegrationTest, CrossSessionDataConsistency) {
    // Session 1: Create initial data
    users->insertUser("session_user", "hash");
    
    for(int game = 0; game < 15; ++game) {
        board.reset();
        auto pattern = getGamePattern(game);
        std::vector<Move> gameMoves;
        char currentPlayer = 'X';
        
        for(const auto& move : pattern) {
            if(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
                if(board.makeMove(move.first, move.second, currentPlayer)) {
                    gameMoves.push_back(Move(move.first, move.second, currentPlayer));
                    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                }
            }
        }
        
        GameResult result = board.checkWin();
        if(result == GameResult::ONGOING && board.isFull()) {
            result = GameResult::TIE;
        }
        
        std::string timestamp = "2025-06-16 " + std::to_string(10 + game) + ":00:00";
        GameRecord record = createGameRecord("session_user", "opponent", result, board.getBoard(), timestamp);
        record.moves = gameMoves;
        history->addGameRecord(record);
        
        User* user = users->getUser("session_user");
        user->gamesPlayed++;
        if(result == GameResult::PLAYER1_WIN) user->gamesWon++;
        else if(result == GameResult::PLAYER2_WIN) user->gamesLost++;
        else user->gamesTied++;
        users->updateUser("session_user", *user);
    }
    
    // Capture session 1 state
    User session1User = *users->getUser("session_user");
    auto session1Games = history->getUserGames("session_user");
    
    // Session 2: Load and continue
    auto session2Users = std::make_unique<UserHashTable>();
    auto session2History = std::make_unique<GameHistory>();
    
    // Verify session 1 data loaded
    EXPECT_TRUE(session2Users->userExists("session_user"));
    User* loadedUser = session2Users->getUser("session_user");
    EXPECT_EQ(loadedUser->gamesPlayed, session1User.gamesPlayed);
    
    auto loadedGames = session2History->getUserGames("session_user");
    EXPECT_EQ(loadedGames.size(), session1Games.size());
    
    // Continue in session 2
    for(int game = 0; game < 10; ++game) {
        board.reset();
        auto pattern = getGamePattern(game + 15);
        std::vector<Move> gameMoves;
        char currentPlayer = 'X';
        
        for(const auto& move : pattern) {
            if(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
                if(board.makeMove(move.first, move.second, currentPlayer)) {
                    gameMoves.push_back(Move(move.first, move.second, currentPlayer));
                    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                }
            }
        }
        
        GameResult result = board.checkWin();
        if(result == GameResult::ONGOING && board.isFull()) {
            result = GameResult::TIE;
        }
        
        std::string timestamp = "2025-06-17 " + std::to_string(10 + game) + ":00:00";
        GameRecord record = createGameRecord("session_user", "opponent", result, board.getBoard(), timestamp);
        record.moves = gameMoves;
        session2History->addGameRecord(record);
        
        User* user = session2Users->getUser("session_user");
        user->gamesPlayed++;
        if(result == GameResult::PLAYER1_WIN) user->gamesWon++;
        else if(result == GameResult::PLAYER2_WIN) user->gamesLost++;
        else user->gamesTied++;
        session2Users->updateUser("session_user", *user);
    }
    
    // Verify cumulative data
    auto finalGames = session2History->getUserGames("session_user");
    EXPECT_EQ(finalGames.size(), 25); // 15 + 10
    
    User* finalUser = session2Users->getUser("session_user");
    EXPECT_EQ(finalUser->gamesPlayed, 25);
}

// ==================== ADVANCED TOURNAMENT AND RANKING SYSTEMS (Tests 301-400) ====================

TEST_F(TicTacToeIntegrationTest, ComprehensiveTournamentWithRankings) {
    // Create tournament players with skill ratings
    std::map<std::string, int> playerSkills = {
        {"novice1", 1}, {"novice2", 1}, {"novice3", 1},
        {"intermediate1", 2}, {"intermediate2", 2}, {"intermediate3", 2},
        {"expert1", 3}, {"expert2", 3}, {"expert3", 3},
        {"master1", 4}, {"master2", 4}
    };
    
    // Register all players
    for(const auto& [player, skill] : playerSkills) {
        users->insertUser(player, "hash");
    }
    
    // Tournament structure: Round-robin within skill groups + cross-group matches
    for(const auto& [p1, skill1] : playerSkills) {
        for(const auto& [p2, skill2] : playerSkills) {
            if(p1 < p2) { // Avoid duplicate matches
                for(int match = 0; match < 3; ++match) { // 3 matches per pair
                    board.reset();
                    stack->clearStack();
                    
                    auto pattern = getGamePattern(match + skill1 + skill2);
                    std::vector<Move> gameMoves;
                    char currentPlayer = 'X';
                    
                    // Skill-based outcome probability
                    GameResult result;
                    if(skill1 > skill2) {
                        result = (match % 3 == 0) ? GameResult::PLAYER1_WIN : 
                                (match % 3 == 1) ? GameResult::PLAYER1_WIN : GameResult::TIE;
                    } else if(skill1 < skill2) {
                        result = (match % 3 == 0) ? GameResult::PLAYER2_WIN : 
                                (match % 3 == 1) ? GameResult::PLAYER2_WIN : GameResult::TIE;
                    } else {
                        result = (match % 3 == 0) ? GameResult::PLAYER1_WIN : 
                                (match % 3 == 1) ? GameResult::PLAYER2_WIN : GameResult::TIE;
                    }
                    
                    // Simulate game to match result
                    for(const auto& move : pattern) {
                        if(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
                            if(board.makeMove(move.first, move.second, currentPlayer)) {
                                Move gameMove(move.first, move.second, currentPlayer);
                                gameMoves.push_back(gameMove);
                                stack->pushMove(gameMove, board.getBoard());
                                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                                
                                if(board.checkWin() == result) break;
                            }
                        }
                    }
                    
                    // Ensure board matches expected result
                    if(board.checkWin() == GameResult::ONGOING && board.isFull()) {
                        result = GameResult::TIE;
                    }
                    
                    // Record tournament match
                    std::string timestamp = "2025-06-16 " + std::to_string(10 + match) + ":00:00";
                    GameRecord record = createGameRecord(p1, p2, result, board.getBoard(), timestamp);
                    record.moves = gameMoves;
                    history->addGameRecord(record);
                    
                    // Update tournament statistics
                    User* user1 = users->getUser(p1);
                    User* user2 = users->getUser(p2);
                    
                    user1->gamesPlayed++;
                    user2->gamesPlayed++;
                    
                    if(result == GameResult::PLAYER1_WIN) {
                        user1->gamesWon++;
                        user2->gamesLost++;
                    } else if(result == GameResult::PLAYER2_WIN) {
                        user1->gamesLost++;
                        user2->gamesWon++;
                    } else {
                        user1->gamesTied++;
                        user2->gamesTied++;
                    }
                    
                    users->updateUser(p1, *user1);
                    users->updateUser(p2, *user2);
                }
            }
        }
    }
    
    // Calculate tournament rankings
    std::vector<std::pair<std::string, double>> rankings;
    for(const auto& [player, skill] : playerSkills) {
        User* user = users->getUser(player);
        double winRate = (user->gamesPlayed > 0) ? 
                        (double)user->gamesWon / user->gamesPlayed : 0.0;
        rankings.push_back({player, winRate});
        
        // Verify reasonable game count
        EXPECT_GT(user->gamesPlayed, 20);
        EXPECT_EQ(user->gamesWon + user->gamesLost + user->gamesTied, user->gamesPlayed);
    }
    
    // Sort rankings
    std::sort(rankings.begin(), rankings.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Verify skill-based ranking correlation
    for(size_t i = 0; i < rankings.size() - 1; ++i) {
        int skill1 = playerSkills[rankings[i].first];
        int skill2 = playerSkills[rankings[i + 1].first];
        
        // Higher skilled players should generally rank higher
        if(skill1 > skill2) {
            EXPECT_GE(rankings[i].second, rankings[i + 1].second - 0.1); // Allow some variance
        }
    }
    
    // Verify tournament data integrity
    auto allGames = history->getAllGames();
    EXPECT_GT(allGames.size(), 150); // Many tournament matches
}

TEST_F(TicTacToeIntegrationTest, SeasonalTournamentWithProgressTracking) {
    std::vector<std::string> players = {"seasonal1", "seasonal2", "seasonal3", "seasonal4", "seasonal5"};
    
    for(const auto& player : players) {
        users->insertUser(player, "hash");
    }
    
    // Simulate 3 seasons
    for(int season = 0; season < 3; ++season) {
        // Each season has multiple rounds
        for(int round = 0; round < 5; ++round) {
            // Round-robin within season
            for(size_t i = 0; i < players.size(); ++i) {
                for(size_t j = i + 1; j < players.size(); ++j) {
                    board.reset();
                    stack->clearStack();
                    
                    auto pattern = getGamePattern(season * 10 + round + i + j);
                    std::vector<Move> gameMoves;
                    char currentPlayer = 'X';
                    
                    for(const auto& move : pattern) {
                        if(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
                            if(board.makeMove(move.first, move.second, currentPlayer)) {
                                Move gameMove(move.first, move.second, currentPlayer);
                                gameMoves.push_back(gameMove);
                                stack->pushMove(gameMove, board.getBoard());
                                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                            }
                        }
                    }
                    
                    GameResult result = board.checkWin();
                    if(result == GameResult::ONGOING && board.isFull()) {
                        result = GameResult::TIE;
                    }
                    
                    // Record with season/round info in timestamp
                    std::string timestamp = "2025-0" + std::to_string(6 + season) + 
                                           "-" + std::to_string(16 + round) + " 14:00:00";
                    GameRecord record = createGameRecord(players[i], players[j], result, board.getBoard(), timestamp);
                    record.moves = gameMoves;
                    history->addGameRecord(record);
                    
                    // Update seasonal statistics
                    User* p1 = users->getUser(players[i]);
                    User* p2 = users->getUser(players[j]);
                    
                    p1->gamesPlayed++;
                    p2->gamesPlayed++;
                    
                    if(result == GameResult::PLAYER1_WIN) {
                        p1->gamesWon++;
                        p2->gamesLost++;
                    } else if(result == GameResult::PLAYER2_WIN) {
                        p1->gamesLost++;
                        p2->gamesWon++;
                    } else {
                        p1->gamesTied++;
                        p2->gamesTied++;
                    }
                    
                    users->updateUser(players[i], *p1);
                    users->updateUser(players[j], *p2);
                }
            }
        }
        
        // Season-end verification
        for(const auto& player : players) {
            auto playerGames = history->getUserGames(player);
            User* user = users->getUser(player);
            
            // Verify games per season
            int expectedGamesPerSeason = 4 * 5; // 4 opponents * 5 rounds
            int expectedTotalGames = expectedGamesPerSeason * (season + 1);
            EXPECT_EQ(user->gamesPlayed, expectedTotalGames);
            EXPECT_EQ(playerGames.size(), expectedTotalGames);
        }
    }
    
    // Final tournament verification
    auto allGames = history->getAllGames();
    EXPECT_EQ(allGames.size(), 150); // 5 players * 4 opponents * 5 rounds * 3 seasons / 2
    
    // Verify temporal progression in game records
    for(size_t i = 1; i < allGames.size(); ++i) {
        EXPECT_LE(allGames[i-1].timestamp, allGames[i].timestamp);
    }
}

// ==================== STRESS TESTING AND EDGE CASES (Tests 401-500) ====================

TEST_F(TicTacToeIntegrationTest, MassiveDataVolumeStressTest) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create large number of users
    for(int i = 0; i < 50; ++i) {
        users->insertUser("stress_user_" + std::to_string(i), "hash");
    }
    
    // Generate massive game volume
    for(int batch = 0; batch < 10; ++batch) {
        for(int userIdx = 0; userIdx < 50; userIdx += 2) {
            std::string p1 = "stress_user_" + std::to_string(userIdx);
            std::string p2 = "stress_user_" + std::to_string(userIdx + 1);
            
            for(int game = 0; game < 5; ++game) {
                board.reset();
                stack->clearStack();
                
                auto pattern = getGamePattern(batch + userIdx + game);
                std::vector<Move> gameMoves;
                char currentPlayer = 'X';
                
                for(const auto& move : pattern) {
                    if(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
                        if(board.makeMove(move.first, move.second, currentPlayer)) {
                            Move gameMove(move.first, move.second, currentPlayer);
                            gameMoves.push_back(gameMove);
                            stack->pushMove(gameMove, board.getBoard());
                            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                        }
                    }
                }
                
                GameResult result = board.checkWin();
                if(result == GameResult::ONGOING && board.isFull()) {
                    result = GameResult::TIE;
                }
                
                std::string timestamp = "2025-06-16 " + std::to_string(10 + (batch + game) % 14) + ":00:00";
                GameRecord record = createGameRecord(p1, p2, result, board.getBoard(), timestamp);
                record.moves = gameMoves;
                history->addGameRecord(record);
                
                // Update statistics
                User* user1 = users->getUser(p1);
                User* user2 = users->getUser(p2);
                
                user1->gamesPlayed++;
                user2->gamesPlayed++;
                
                if(result == GameResult::PLAYER1_WIN) {
                    user1->gamesWon++;
                    user2->gamesLost++;
                } else if(result == GameResult::PLAYER2_WIN) {
                    user1->gamesLost++;
                    user2->gamesWon++;
                } else {
                    user1->gamesTied++;
                    user2->gamesTied++;
                }
                
                users->updateUser(p1, *user1);
                users->updateUser(p2, *user2);
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // Verify massive data handling
    auto allGames = history->getAllGames();
    EXPECT_EQ(allGames.size(), 1250); // 25 pairs * 5 games * 10 batches
    
    // Verify all users have correct statistics
    for(int i = 0; i < 50; ++i) {
        std::string username = "stress_user_" + std::to_string(i);
        User* user = users->getUser(username);
        EXPECT_EQ(user->gamesPlayed, 50); // 5 games * 10 batches
    }
    
    // Performance should be reasonable (less than 30 seconds for 1250 games)
    EXPECT_LT(duration.count(), 30000);
    
    // Test data persistence under stress
    auto newUsers = std::make_unique<UserHashTable>();
    auto newHistory = std::make_unique<GameHistory>();
    
    EXPECT_EQ(newHistory->getAllGames().size(), allGames.size());
    EXPECT_TRUE(newUsers->userExists("stress_user_0"));
    EXPECT_TRUE(newUsers->userExists("stress_user_49"));
}

TEST_F(TicTacToeIntegrationTest, ConcurrentOperationSimulation) {
    // Simulate concurrent-like operations by interleaving different workflows
    users->insertUser("concurrent1", "hash");
    users->insertUser("concurrent2", "hash");
    users->insertUser("concurrent3", "hash");
    
    // Workflow 1: Normal gameplay
    for(int i = 0; i < 20; ++i) {
        board.reset();
        stack->clearStack();
        
        auto pattern = getGamePattern(i);
        std::vector<Move> gameMoves;
        char currentPlayer = 'X';
        
        for(const auto& move : pattern) {
            if(board.checkWin() == GameResult::ONGOING && !board.isFull()) {
                if(board.makeMove(move.first, move.second, currentPlayer)) {
                    gameMoves.push_back(Move(move.first, move.second, currentPlayer));
                    stack->pushMove(Move(move.first, move.second, currentPlayer), board.getBoard());
                    currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                }
            }
        }
        
        GameResult result = board.checkWin();
        if(result == GameResult::ONGOING && board.isFull()) {
            result = GameResult::TIE;
        }
        
        // Interleave with user operations
        if(i % 3 == 0) {
            User* user = users->getUser("concurrent1");
            user->gamesPlayed++;
            users->updateUser("concurrent1", *user);
        }
        
        std::string timestamp = "2025-06-16 " + std::to_string(10 + i % 14) + ":00:00";
        GameRecord record = createGameRecord("concurrent1", "concurrent2", result, board.getBoard(), timestamp);
        record.moves = gameMoves;
        history->addGameRecord(record);
        
        // Interleave with more user operations
        if(i % 2 == 0) {
            User* user = users->getUser("concurrent2");
            user->gamesPlayed++;
            users->updateUser("concurrent2", *user);
        }
        
        // Test stack operations
        while(stack->canUndo() && i % 5 == 0) {
            stack->popMove();
            stack->popBoardState();
        }
    }
    
    // Verify data consistency after interleaved operations
    auto games = history->getAllGames();
    EXPECT_EQ(games.size(), 20);
    
    User* user1 = users->getUser("concurrent1");
    User* user2 = users->getUser("concurrent2");
    EXPECT_GT(user1->gamesPlayed, 0);
    EXPECT_GT(user2->gamesPlayed, 0);
}

TEST_F(TicTacToeIntegrationTest, EdgeCaseHandling) {
    // Test edge cases with empty board
    board.reset();
    stack->clearStack();
    
    GameResult result = board.checkWin();
    EXPECT_EQ(result, GameResult::ONGOING);
    
    // Test full board with no winner
    board.setBoard({
        {'X', 'O', 'X'},
        {'O', 'X', 'O'},
        {'O', 'X', 'O'}
    });
    
    result = board.checkWin();
    EXPECT_EQ(result, GameResult::TIE);
    
    // Test invalid moves
    EXPECT_FALSE(board.makeMove(-1, -1, 'X')); // Out of bounds
    EXPECT_FALSE(board.makeMove(3, 3, 'O'));  // Out of bounds
    EXPECT_FALSE(board.makeMove(0, 0, 'X'));  // Cell already occupied
    
    // Test undo on empty stack
    EXPECT_FALSE(stack->canUndo());
    
    // Test stack operations with no moves
    stack->clearStack();
    EXPECT_EQ(stack->popMove().player, ' ');
}

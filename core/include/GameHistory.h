#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include "GameBoard.h"
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>

enum class GameMode {
    PLAYER_VS_PLAYER,
    PLAYER_VS_AI
};

struct Move {
    int row;
    int col;
    char player;
    std::string timestamp;
    int moveNumber;

    Move() : row(-1), col(-1), player(' '), moveNumber(-1) {}
    Move(int r, int c, char p) : row(r), col(c), player(p), moveNumber(-1) {}
    Move(int r, int c, char p, const std::string& time, int num)
        : row(r), col(c), player(p), timestamp(time), moveNumber(num) {}
};

struct GameRecord {
    std::string player1;
    std::string player2;
    GameMode mode;
    GameResult result;
    std::vector<std::vector<char>> finalBoard;
    std::string timestamp;
    std::vector<Move> moves;

    GameRecord() = default;
    GameRecord(const std::string& p1, const std::string& p2, GameMode m, GameResult r,
               const std::vector<std::vector<char>>& board, const std::string& time)
        : player1(p1), player2(p2), mode(m), result(r), finalBoard(board), timestamp(time) {}
};

class GameHistory {
public:
    GameHistory();
    void addGameRecord(const GameRecord& record);
    std::vector<GameRecord> getUserGames(const std::string& username);
    std::vector<GameRecord> getAllGames();
    void saveHistory();
    void loadHistory();

private:
    std::vector<GameRecord> gameRecords;
    std::string historyFile;
    std::string getCurrentTimestamp();
    void loadHistoryIfNeeded();
};

#endif // GAMEHISTORY_H

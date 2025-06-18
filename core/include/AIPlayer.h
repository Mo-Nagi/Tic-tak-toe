#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "GameBoard.h"
#include <stack>
#include <random>
#include <climits>
#include <limits>

enum class DifficultyLevel {
    EASY,
    MEDIUM,
    HARD
};

class AIPlayer {
public:
    AIPlayer(char aiSymbol, char humanSymbol, DifficultyLevel difficulty = DifficultyLevel::HARD);
    std::pair<int, int> getBestMove(const GameBoard& board);
    void setDifficulty(DifficultyLevel difficulty);
    void pushAIMove(int row, int col);
    std::pair<int, int> popAIMove();
    bool hasAIMoveHistory();
    void clearAIMoveHistory();

private:
    std::pair<int, int> findBestMove(const GameBoard& board, int forcedChance = -1);
    int minimax(GameBoard board, int depth, bool isMax, int alpha, int beta);
    int minimaxLimited(GameBoard board, int depth, bool isMax, int alpha, int beta, int maxDepth);
    std::string difficultyToString(DifficultyLevel diff);

    char aiSymbol;
    char humanSymbol;
    DifficultyLevel currentDifficulty;
    std::stack<std::pair<int, int>> aiMoveHistory;
};

#endif // AIPLAYER_H

#include "AIPlayer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

AIPlayer::AIPlayer(char aiSym, char humanSym, DifficultyLevel difficulty)
    : aiSymbol(aiSym), humanSymbol(humanSym), currentDifficulty(difficulty) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void AIPlayer::setDifficulty(DifficultyLevel difficulty) {
    currentDifficulty = difficulty;
}

void AIPlayer::pushAIMove(int row, int col) {
    aiMoveHistory.push({row, col});
}

std::pair<int, int> AIPlayer::popAIMove() {
    if (!aiMoveHistory.empty()) {
        auto move = aiMoveHistory.top();
        aiMoveHistory.pop();
        return move;
    }
    return {-1, -1};
}

bool AIPlayer::hasAIMoveHistory() {
    return !aiMoveHistory.empty();
}

void AIPlayer::clearAIMoveHistory() {
    while (!aiMoveHistory.empty()) aiMoveHistory.pop();
}

// Check for immediate win or block
static std::pair<int,int> getCriticalMove(const GameBoard& board, char symbol) {
    auto moves = board.getAvailableMoves();
    for (auto [r,c] : moves) {
        GameBoard tmp = board;
        tmp.makeMove(r, c, symbol);
        if (tmp.checkWin() != GameResult::ONGOING) return {r,c};
    }
    return {-1,-1};
}

std::pair<int, int> AIPlayer::getBestMove(const GameBoard& board) {
    // 1. Try immediate win
    auto winMove = getCriticalMove(board, aiSymbol);
    if (winMove.first != -1) {
        pushAIMove(winMove.first, winMove.second);
        return winMove;
    }
    // 2. Block opponent win
    auto blockMove = getCriticalMove(board, humanSymbol);
    if (blockMove.first != -1) {
        pushAIMove(blockMove.first, blockMove.second);
        return blockMove;
    }
    // 3. Otherwise strategic search
    auto move = findBestMove(board);
    if (move.first != -1) pushAIMove(move.first, move.second);
    return move;
}

std::pair<int, int> AIPlayer::findBestMove(const GameBoard& board, int forcedChance) {
    auto moves = board.getAvailableMoves();
    if (moves.empty()) return {-1,-1};

    int chance = (forcedChance >= 0) ? forcedChance : (std::rand() % 100);
    int bestVal = std::numeric_limits<int>::min();
    std::pair<int,int> bestMove = moves[0];

    // Depending on difficulty, randomized chance
    bool useRandom = (currentDifficulty == DifficultyLevel::EASY && chance < 80)
                  || (currentDifficulty == DifficultyLevel::MEDIUM && chance < 40)
                  || (currentDifficulty == DifficultyLevel::HARD && chance < 5);

    if (useRandom) {
        return moves[std::rand() % moves.size()];
    }

    int depthLimit = (currentDifficulty == DifficultyLevel::EASY ? 1
                     : currentDifficulty == DifficultyLevel::MEDIUM ? 2
                     : 9);

    for (auto [r,c] : moves) {
        GameBoard tmp = board;
        tmp.makeMove(r, c, aiSymbol);
        int val = (depthLimit < 9)
            ? minimaxLimited(tmp, 0, false, INT_MIN, INT_MAX, depthLimit)
            : minimax(tmp, 0, false, INT_MIN, INT_MAX);
        if (val > bestVal) {
            bestVal = val;
            bestMove = {r,c};
        }
    }
    return bestMove;
}

int AIPlayer::minimax(GameBoard board, int depth, bool isMax, int alpha, int beta) {
    GameResult result = board.checkWin();
    if (result != GameResult::ONGOING) {
        if (result == GameResult::TIE) return 0;
        bool aiWon = ((result == GameResult::PLAYER1_WIN && aiSymbol=='X')
                   || (result==GameResult::PLAYER2_WIN && aiSymbol=='O'));
        return aiWon ? 10-depth : depth-10;
    }
    if (isMax) {
        int best = INT_MIN;
        for (int r=0;r<3;++r) for (int c=0;c<3;++c) {
            if (board.getCell(r,c)==' ') {
                GameBoard tmp=board; tmp.makeMove(r,c,aiSymbol);
                best = std::max(best, minimax(tmp, depth+1, false, alpha, beta));
                alpha = std::max(alpha, best);
                if (beta<=alpha) return best;
            }
        }
        return best;
    } else {
        int best = INT_MAX;
        for (int r=0;r<3;++r) for (int c=0;c<3;++c) {
            if (board.getCell(r,c)==' ') {
                GameBoard tmp=board; tmp.makeMove(r,c,humanSymbol);
                best = std::min(best, minimax(tmp, depth+1, true, alpha, beta));
                beta = std::min(beta, best);
                if (beta<=alpha) return best;
            }
        }
        return best;
    }
}

int AIPlayer::minimaxLimited(GameBoard board, int depth, bool isMax,
                             int alpha, int beta, int maxDepth) {
    GameResult result = board.checkWin();
    if (result != GameResult::ONGOING) {
        if (result==GameResult::TIE) return 0;
        bool aiWon = ((result==GameResult::PLAYER1_WIN && aiSymbol=='X')
                   || (result==GameResult::PLAYER2_WIN && aiSymbol=='O'));
        return aiWon ? 10-depth : depth-10;
    }
    if (depth>=maxDepth) {
        // heuristic: count center preference
        return (board.getCell(1,1)==aiSymbol?3:0);
    }
    return minimax(board, depth, isMax, alpha, beta);
}

std::string AIPlayer::difficultyToString(DifficultyLevel diff) {
    switch(diff) {
        case DifficultyLevel::EASY: return "easy";
        case DifficultyLevel::MEDIUM: return "medium";
        case DifficultyLevel::HARD: return "hard";
    }
    return "unknown";
}
